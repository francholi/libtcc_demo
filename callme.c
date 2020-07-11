#include <stdio.h>
#include "callme.h"

void callMe(const char* str)
{
    fprintf(stderr, "ello from callMe, %s\n", str);
}