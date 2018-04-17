/*
 * dynamic module.
 * symbols coming from libraries are automatically accessible.
 * symbols coming from other places (global vars and compiled functions)
 * need to be added manually by the code that compiles this module.
 * 
 * I did not remember that in C, a signature of a function does not matter,
 * only the function name :)
 * Easy to import/export stuff, easy to crash as well...
 * 
 * eg. globalState is a global variable from main.c (needs adding)
 *     compile_program is a function coming from shader.c (needs adding)
 *    
 *     glClear is coming from library (no need adding)
 */

#include "libtcc.h"
#include <stdio.h>
#include <math.h>
#include "shader.h"
#include "global_state_def.h"

#include "level0.h"

extern GlobalState* globalState;

// every time the program is reloaded (recompiled), these go away!
// so care must be taken freeing up any resource.
// the pattern is basically, "unload -> recompile -> load -> use"

// my load and unload functions follow some convention, but it is entirely
// arbitrary. So load_filename and unload_filename should be present for this
// to work with any other files that are to be treated this way.
static GLuint vs_shader = 0;
static GLuint fs_shader = 0;
static GLuint program = 0;

static GLuint VAO = 0;

static unsigned long long slow_down = 0;
int render() {
  if (slow_down % 30 == 0) {
    recompile_program("vs.vert", "fs.frag", &vs_shader, &fs_shader, &program);
    glUseProgram(program);
  }
  glClearColor(1.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  slow_down++;
  return 0;
};

int load_level0() {
    recompile_program("vs.vert", "fs.frag", &vs_shader, &fs_shader, &program);
    glUseProgram(program);
    // dummy VAO to generate gl_VertexID
		glGenVertexArrays(1, &VAO);
	  glBindVertexArray(VAO);
};

int unload_level0() {
    // delete vertex buffer
    printf("deleting shaders\n");
    glDeleteShader( vs_shader );
    glDeleteShader( fs_shader );
    printf("deleting program\n");
    glDeleteProgram( program );
    glDeleteVertexArrays(1, &VAO);
};
