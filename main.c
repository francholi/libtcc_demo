#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "platform.h"
#include "driver.h"
#include "global_state_def.h"

GlobalState* gs;

// driver pointers
void (*start_hot)() = start;


/*
 * gets called when a file has been changed
 */
void file_changed_cb(const char* filename)
{
    fprintf(stderr,"file %s changed!\n", filename);
}

int main(int argc, char** argv) {
    // some global state to share around.
    gs = (GlobalState*)malloc(sizeof(GlobalState));

    start_hot();
    start();
//    while(1)
//    {
//    }
    fprintf(stderr, "success\n");
    char c = getchar();
};

