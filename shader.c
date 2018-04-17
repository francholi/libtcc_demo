#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct timestamp { 
    char name[100];  
    time_t last_modified;
} timestamp;

static timestamp file_stamps[100];
static int file_stamps_count = 0;



int shader_file_changed(const char* file_name) {
    struct stat result;
    int count = 0;
    // file exists?
    if (stat(file_name, &result) == 0)
    {
        while(count < file_stamps_count)
        {
            // file registered?
            if (strcmp(file_name, file_stamps[count].name) == 0) {
                // file modified?
                if (file_stamps[count].last_modified != result.st_mtime)
                {
                    file_stamps[count].last_modified = result.st_mtime;
                    return 1;
                }
                else
                    return 0;
            }   
            count++;
        }
        // file not registered. hardcoded to 100 files.
        if (count == file_stamps_count)
        {
            file_stamps[count].last_modified = result.st_mtime;
            strncpy(file_stamps[count].name, file_name, 100);
            file_stamps_count++;
            return 1;
        }
    }
    else {
        printf("File not found: %s\n", file_name);
    }
    return -1;
};

/*
 * return -1: file does not exist;
 * return 0: shader did not compile.
 * return >0: if shader has been compiled, returns shader id.
 */
GLint compile_shader_from_file(const char* file, GLuint type) {

    FILE* shader_file = fopen(file,"r");
    if (shader_file == NULL)
    {
        fprintf(stderr, "file does not exist: %s\n", file);
        return -1;
    }

    GLchar* content = (char*)malloc(16*1024);
    memset(content, 0, 16*1024);

    fread(content, 1, 16*1024, shader_file);

    GLuint new_shader = glCreateShader(type);

    glShaderSource(new_shader, 1, (const GLchar* const*)&content, NULL );
    glCompileShader(new_shader);

    GLint compile_result = 0;
    glGetShaderiv(new_shader, GL_COMPILE_STATUS, &compile_result);
    if (compile_result == GL_FALSE)
    {
        char buff[1024];
        fprintf(stderr, "%s\n", content);
        fflush(NULL);
        memset(buff,0,1024);
        glGetShaderInfoLog(new_shader, 1024, NULL, buff);
        fprintf(stderr, "%s\n", buff);
        glDeleteShader(new_shader);
        new_shader = 0;
    }
    fclose(shader_file);
    free(content);

    return new_shader;
}

// only if compilation succeeds, shader_id pointer is updated.
int recompile_shader(const char* file, GLuint *shader_id, GLuint type) {
    // if not valid shader, try to compile.
    if (*shader_id == 0)
    {
        *shader_id = compile_shader_from_file(file, type);
        return 0;
    }

    // we had a valid shader already, check if file is newer.
    int dirty = shader_file_changed(file);
    
    if (dirty == 0) // file has not changed, we have a valid shader != 0
        return 0;

    if (dirty == -1) // file not found. dirty == -1
        return -1;

    // here, old shader is valid, and file has changed. try to recompile.
    GLint new_shader = compile_shader_from_file(file, type);

    // shader file has not changed, and have a valid shader.
    if (new_shader > 0) {
        printf("Shader recompiled successfuly: %s\n", file);
        glDeleteShader(*shader_id);
        *shader_id = new_shader;
        return new_shader; // success
    }
    else
        return -1; // error
};

// 1 is success. 0 is no change. -1 is error.
int recompile_program(const char* vs, const char* fs, GLuint* vs_id, GLuint* fs_id, GLuint *program_id) {
    GLuint new_program = 0;

    int res_vs = recompile_shader(vs, vs_id, GL_VERTEX_SHADER);
    int res_fs = recompile_shader(fs, fs_id, GL_FRAGMENT_SHADER);

    // some error
    if ((res_vs == -1 || res_fs == -1))
        return -1;

    // files have not changed
    if (res_vs == 0 && res_fs == 0 && *program_id != 0)
        return 0;

    new_program = glCreateProgram();
    glAttachShader(new_program, *vs_id);
    glAttachShader(new_program, *fs_id);

    glLinkProgram(new_program);

    glDetachShader(new_program, *vs_id);
    glDetachShader(new_program, *fs_id);

    GLint link_result = 0;
    glGetProgramiv(new_program, GL_LINK_STATUS, &link_result);
    if (link_result == GL_FALSE) {
        char buff[1024];
        memset(buff,0,1024);
        glGetProgramInfoLog(new_program, 1024, NULL, buff);
        fprintf(stdout, "%s\n", buff);
        fflush(NULL);
        glDeleteProgram(new_program);
        new_program = 0;
        return -1;
    };
    printf("Program recompiled successfuly\n");
    // delete old program
    glDeleteProgram(*program_id);
    *program_id = new_program;
    return 1;
}