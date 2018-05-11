#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global_state_def.h"
#include "module_loader.h"

// get POINTERS to functions definitions from here.
// this is gonna be loaded dynamically
#include "level0.h"

#define KEYPRESS { char c = getchar(); }

#define HOT_RELOAD
#ifdef HOT_RELOAD
#define FUNC_CALL(F) { if (F) F(); else printf("Error calling ##F\n"); }
#define RELOAD_SYMBOLS() if (reload_symbols() == 0) { printf("error reloading symbols\n"); }
#else
#define RELOAD_SYMBOLS()
#endif

// this symbol is added to all modules loaded
GlobalState* gs;

void init_window(int width, int height, const char* title);
void glfw_error_callback(int error, const char* description);

static int quit = 0;
int main(int argc, char** argv) {

    // some global state to share around.
    gs = (GlobalState*)malloc(sizeof(GlobalState));

    init_window(600,600, "testing_live_123");

    // it would be nice to not pollute the code like this...
    #ifdef HOT_RELOAD
      render_level0_ptr render = 0;
      load_level0_ptr load_level0 = 0;
      unload_level0_ptr unload_level0 = 0;
      // if level0.c is recompiled, get new pointers to these functions
      watch_symbol("render", (void**)&render, "level0.c");
      watch_symbol("load_level0", (void**)&load_level0, "level0.c");
      watch_symbol("unload_level0", (void**)&unload_level0, "level0.c");
      RELOAD_SYMBOLS();
    #endif

    FUNC_CALL(load_level0);

    while (!glfwWindowShouldClose(gs->glfwWindow) && !quit)
    { 
      // poll events
      glfwPollEvents();
      quit = glfwGetKey(gs->glfwWindow, GLFW_KEY_ESCAPE);
      RELOAD_SYMBOLS();
      FUNC_CALL(render);
      // present
      glfwSwapBuffers(gs->glfwWindow);
    };

    if (unload_level0)
      unload_level0();

    glfwDestroyWindow(gs->glfwWindow);
    glfwTerminate();
    KEYPRESS;
};

/* Create GLFW window */
void init_window(int width, int height, const char* title) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *w = glfwCreateWindow(width, height, "basic GLFW window", NULL, NULL);
  if (!w)
  {
    fprintf(stderr, "error creating window\n");
    exit(-1);
  }
  glfwMakeContextCurrent(w);
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    fprintf(stderr, "Error GLEW: %s\n", glewGetErrorString(err));
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

  gs->glfwWindow = w;
  return;
}

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    return;
};
