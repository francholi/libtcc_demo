#ifndef GLOBAL_DEFS
#define GLOBAL_DEFS

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// TODO: this is some idea for communication between modules,
// not implemented yet. But goes with the global state everywhere.
typedef struct Messenger {} Messenger;

typedef struct GlobalState {
  Messenger messenger;
  GLFWwindow* glfwWindow;

} GlobalState;

#endif


