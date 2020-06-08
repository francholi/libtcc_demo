#include "global_state_def.h"


// typedef int(*render_level0_ptr)();
typedef int(*function_pointer)();
//typedef int(*load_level0_ptr)();

function_pointer render0;
function_pointer load_level0;
function_pointer unload_level0;

//  int render();
//  int load_level0();
//  int unload_level0();


