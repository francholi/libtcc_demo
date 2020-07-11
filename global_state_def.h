#ifndef GLOBAL_DEFS
#define GLOBAL_DEFS

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>

typedef struct GlobalState {
  //Messenger messenger;
  //GLFWwindow* glfwWindow;
  int counter;
  int break_main_loop;
  int break_exit_application;

  int break_start_loop;
  int driver_wants_reload;

} GlobalState;

#endif


