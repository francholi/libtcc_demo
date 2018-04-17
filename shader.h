#include <GL/glew.h>
GLint compile_shader_from_file(const char* file, GLuint type);
int recompile_shader(const char* file, GLuint *shader_id, GLuint type);
int recompile_program(const char* vs, const char* fs, GLuint* vs_id, GLuint* fs_id, GLuint *program_id);


