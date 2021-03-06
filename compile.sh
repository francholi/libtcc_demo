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

# file_utils.c
if [ file_utils.c -nt file_utils.o ]; then 
    cc -g -c file_utils.c -o file_utils.o
fi

# file_watcher.c
if [ file_watcher.c -nt file_watcher.o ]; then 
    cc -g -c file_watcher.c -o file_watcher.o
fi


cc -g main.c platform.o shader.o file_utils.o file_watcher.o module_loader.o -L. -lm -lpthread -ltcc -lglfw -lGL -lGLEW -ldl -o main

