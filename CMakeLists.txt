cmake_minimum_required(VERSION 3.16)
project(libtcc_demo C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(libtcc_demo
        file_utils.c
        file_utils.h
        file_watcher.c
        file_watcher.h
        global_state_def.h
        level0.c
        level0.h
        libtcc.h
        main.c
        module_loader.c
        module_loader.h
        platform.c
        platform.h
        shader.c
        shader.h)
