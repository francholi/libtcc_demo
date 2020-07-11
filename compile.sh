#!/bin/bash

function stop_err {
	if [ $? -ne 0 ]; then
		exit 1
	fi
}

# shader.c
if [ shader.c -nt shader.o ]; then 
    cc -Wall -g -c shader.c -o shader.o
    stop_err
fi

# platform.c
if [ platform.c -nt platform.o ]; then 
    cc -Wall -g -c platform.c -o platform.o
    stop_err
fi

# module_loader.c
if [ module_loader.c -nt module_loader.o ]; then 
    cc -g -c module_loader.c -o module_loader.o
    stop_err
fi

# file_utils.c
if [ file_utils.c -nt file_utils.o ]; then 
    cc -g -c file_utils.c -o file_utils.o
    stop_err
fi

# file_watcher.c
if [ file_watcher.c -nt file_watcher.o ]; then 
    cc -g -c file_watcher.c -o file_watcher.o
    stop_err
fi

# file_level0.c
if [ level0.c -nt level0.o ]; then 
    cc -g -c level0.c -o level0.o
    stop_err
fi

# driver.c
if [ driver.c -nt driver.o ]; then
    cc -g -c driver.c -o driver.o
    stop_err
fi

# main.c
if [ main.c -nt main.o ]; then
    cc -g -c main.c -o main.o
    stop_err
fi

#cc -g level0.o main.o platform.o shader.o file_utils.o file_watcher.o module_loader.o -L. -lm -lpthread -ltcc -lglfw -lGL -lGLEW -ldl -o main
cc -g main.o driver.o callme.c file_utils.o file_watcher.o module_loader.o -L. -lm -lpthread -ltcc -lglfw -lGL -lGLEW -ldl -o main

