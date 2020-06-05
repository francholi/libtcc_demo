#!/bin/bash

# shader.c
if [ shader.c -nt shader.o ]; then 
    cc -Wall -g -c shader.c -o shader.o
fi

# platform.c
if [ platform.c -nt platform.o ]; then 
    cc -Wall -g -c platform.c -o platform.o
fi

# module_loader.c
if [ module_loader.c -nt module_loader.o ]; then 
    cc -g -c module_loader.c -o module_loader.o
fi

cc -g main.c platform.o shader.o module_loader.o -L. -lm -ltcc -lglfw -lGL -lGLEW -ldl -o main 

