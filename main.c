#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "file_watcher.h"
#include "driver.h"
#include "global_state_def.h"
#include "module_loader.h"

GlobalState* gs;
void init_global_state(GlobalState *gs);
void dump_global_state(GlobalState *gs);

// pointers to functions from driver.c (hot_reloaded)
void(*start_hot)() = start;
#define start start_hot

void(*load_module_driver_hot)() = load_module_driver;
#define load_module_driver load_module_driver_hot

void(*unload_module_driver_hot)() = unload_module_driver;
#define unload_module_driver unload_module_driver_hot

// this is called from a separate thread,
// we recompile from here, without patching pointers.
int file_changed_cb(const char* filename) {

    // try to compile and relocate/link the new code,
    // still cant callit until we patch pointers.
    int RES = reload_symbols(filename);
    if (RES == 0)
    {
        gs->break_start_loop = 1;
    }
    return 0;
}

int main(int argc, char** argv) {

    // start a file_watcher thread, that will notify via cb of file changes
    struct file_watcher_ctx* file_watcher_ctx = fw_init();
    fw_watch(file_watcher_ctx, "driver.c", file_changed_cb);

    // register what symbols we are going to relocate/patch when code changes.
    // initially they point to original start/stop
    register_symbol("start", (void**)&start, "driver.c", 0);
    register_symbol("load_module_driver", (void**)&load_module_driver, "driver.c", 0);
    register_symbol("unload_module_driver", (void**)&unload_module_driver, "driver.c", 0);

    // mega global state, with RAM database (key,value) for state recording
    // for anyone. Each module can use this at will...with a little api.
    gs = (GlobalState*)malloc(sizeof(GlobalState));
    memset(gs, 0, sizeof(gs));
    // create a connection to a redis db
    init_global_state(gs);

    while (1)
    {
        load_module_driver();
        start();
        unload_module_driver();
        patch_pointers("driver.c");
    }
        

    dump_global_state(gs);
    free(gs); // think carefully about this. redis?

    fprintf(stderr, "bye bye for now!\n");

    fw_stop_watch(file_watcher_ctx, "driver.c");
    fw_shutdown(file_watcher_ctx);
};

void init_global_state(GlobalState *gs) { }
void dump_global_state(GlobalState *gs) { }


