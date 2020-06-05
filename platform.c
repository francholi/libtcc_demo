#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "platform.h"

// #define INPUT_KEY_ESCAPE_MAP GLFW_KEY_ESCAPE

void static glfw_error_callback(int error, const char* description);
static GLFWwindow* g_window_handle_native;

/* Create GLFW window */
void window_init(int width, int height, const char* title) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  g_window_handle_native = glfwCreateWindow(width, height, "platformer", NULL, NULL);

  if (!g_window_handle_native)
  {
    fprintf(stderr, "Error creating platform window\n");
    exit(-1);
  }

  glfwMakeContextCurrent(g_window_handle_native);
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();

  if (GLEW_OK != err)
  {
    fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
  }

  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *version = glGetString(GL_VERSION);
  fprintf(stderr, "Renderer: %s\n", renderer);
  fprintf(stderr, "OpenGL version %s\n", version);
  glClearDepth(1.0f);
  glEnable(GL_CULL_FACE);
  glfwSwapInterval(1);
  glfwSetErrorCallback(glfw_error_callback);
  glViewport(0,0, width, height);

  // window_handle.handle = window_handle_native; 
  // gs->glfwWindow = window_handle_native;
  return;
}

bool window_should_close()
{
  return (bool)glfwWindowShouldClose(g_window_handle_native);
};

void input_poll() {
    glfwPollEvents();
};

void window_swap_buffers() {
    glfwSwapBuffers(g_window_handle_native);
};

void window_destroy(){ 
    glfwDestroyWindow(g_window_handle_native);
    glfwTerminate();
}

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    return;
};


bool input_key_pressed(int key)
{
  return (bool) glfwGetKey(g_window_handle_native, key)==GLFW_PRESS;
}

bool input_key_released(int key)
{
  return (bool) glfwGetKey(g_window_handle_native, key)==GLFW_RELEASE;
}

/*
char input_get_key(char key)
{
  char code = 0;
  if (key == INPUT_KEY_ESCAPE) code = INPUT_KEY_ESCAPE_MAP;

  return glfwGetKey(g_window_handle_native, code);
}
*/
/*
 struct Window_Handle {
    GLFWwindow* handle;
} window_handle;
// static Window_Handle g_window_handle;
Window_Handle* window_get_platform_handle()
{
    return &window_handle;
}
*/

