# libtcc_demo
Small C example using libtcc, for C and GLSL live code editing
My code has no license, or do whatever you want with it.
Other parts (glfw, glew, libtcc) have their licenses...

Only linux, but it does not have many dependencies, so it should not be hard to port to other OS's.
For linux you would have to install libtcc (I only tested on ubuntu flavours and it works fine).

The compile script assumes libraries installed system wide.
Try compiling with compile.sh (just cc calls), then run main, and then editing the file level0.c and fs.frag. 

Changes should be hot-reloaded when saving.

Then look at main.c and module_loader.c for details, I added some comments to explain what is going on.

