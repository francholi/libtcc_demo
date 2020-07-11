#include <libtcc.h>
#include <stdio.h>
#include <stdbool.h>
#include "driver.h"
#include "global_state_def.h"
#include "module_loader.h"

extern GlobalState* gs;


// called before anything.
void load_module_driver();
// called if lib is dispossed or reloaded.
void unload_module_driver();

// only one symbol is public here.
void start(void)
{
    // this will be changed from the callback in main
    // from another thread (filewatcher)
    while(gs->break_start_loop == 0) {
        printf("another test\n");
        sleep(2);
    }
};

void load_module_driver()
{
    gs->break_start_loop = 0;
    fprintf(stderr, "loading module driver\n");
}

void unload_module_driver()
{
	printf("unload w\n");
};


