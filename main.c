#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "platform.h"

#include "global_state_def.h"
#include "module_loader.h"
#include "file_watcher.h"

// get POINTERS to functions definitions from here.
// this is gonna be loaded dynamically
#include "level0.h"

#define KEYPRESS { char c = getchar(); }

#define HOT_RELOAD
#ifdef HOT_RELOAD
#define FUNC_CALL(F) { if (F) F(); else printf("Error calling ##F\n"); }
#define RELOAD_SYMBOLS() if (reload_symbols() == 0) { printf("error reloading symbols\n"); }
#else
#define RELOAD_SYMBOLS()
#endif

// this symbol is added to all modules loaded
GlobalState* gs;


void file_changed_callback(const char* filename)
{
    fprintf(stderr,"file %s changed!\n", filename);
}

void file_changed_callback2(const char* filename)
{
    fprintf(stderr,"file %s changed 22222222222!\n", filename);
    fflush(stderr);
}

static bool quit = false;
int main(int argc, char** argv) {
    // some global state to share around.
    gs = (GlobalState*)malloc(sizeof(GlobalState));

    window_init(600,600, "testing_live_123");

    // it would be nice to not pollute the code like this...
    #ifdef HOT_RELOAD
      render_level0_ptr render = 0;
      load_level0_ptr load_level0 = 0;
      unload_level0_ptr unload_level0 = 0;
      // if level0.c is recompiled, get new pointers to these functions
      watch_symbol("render", (void**)&render, "level0.c");
      watch_symbol("load_level0", (void**)&load_level0, "level0.c");
      watch_symbol("unload_level0", (void**)&unload_level0, "level0.c");
      RELOAD_SYMBOLS();
    #endif

    FUNC_CALL(load_level0);

    //while (!glfwWindowShouldClose(gs->glfwWindow) && !quit)
    while (!window_should_close() && !quit)
    { 
      // poll events
      // glfwPollEvents();
      input_poll();

      quit = input_key_pressed(INPUT_KEY_ESCAPE);

      RELOAD_SYMBOLS();
      FUNC_CALL(render);
      // present
      window_swap_buffers();
      // glfwSwapBuffers(gs->glfwWindow);
    };

    if (unload_level0)
      unload_level0();

    window_destroy();

    KEYPRESS;
};

