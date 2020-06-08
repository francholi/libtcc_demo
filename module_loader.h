#include <libtcc.h>
#include "global_state_def.h"

typedef enum RUNMODE {
  ONCE,
  LOOP,
  LOOP_RELOAD
} RUNMODE;

// typedef TCCState Context;

static void tcc_err_callback(void* err_opaque, const char* msg);
int reload_symbols();
int watch_symbol(const char* symbol_name, void** func_pointer, const char* file_name, char hotreload);

// get address of visible variable or function
// void* get_symbol( Context* ctx, const char* module_name, const char* symbol_name);
// set address of variable or function
// void add_symbol(Context* ctx, const char* name, void* pointer);

// int run_module(Context* ctx, RUNMODE mode);
// int unload_module(Context* ctx, const char* name);


