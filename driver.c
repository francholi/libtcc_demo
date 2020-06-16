#include <libtcc.h>
#include <stdio.h>
#include "driver.h"
#include "global_state_def.h"

extern GlobalState* gs;

void start(void)
{
    fprintf(stderr, "hello from start()\n");
    fprintf(stderr, "gs counter: %d\n", gs->counter);
}


