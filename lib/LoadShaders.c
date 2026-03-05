//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.c ---
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// Asegúrate de incluir tu cargador de OpenGL aquí.
// El código original usaba gl3w. Si usas GLEW o GLAD, cambia esto.
#include <GL/gl3w.h> 
#include "LoadShaders.h"

//----------------------------------------------------------------------------

static const GLchar*
ReadShader(const char* filename)
{
    FILE* infile;

#ifdef _WIN32
    if (fopen_s(&infile, filename, "rb") != 0) {
        infile = NULL;
    }
#else
    infile = fopen(filename, "rb");
#endif

    if (!infile) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        return NULL;
    }

    fseek(infile, 0, SEEK_END);
    long len = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    // Asignación de memoria estilo C
    GLchar* source = (GLchar*)malloc(len + 1);
    if (!source) {
        fprintf(stderr, "Memory allocation failed for '%s'\n", filename);
        fclose(infile);
        return NULL;
    }

    fread(source, 1, len, infile);
    fclose(infile);

    source[len] = 0; // Null-terminator

    return (const GLchar*)source;
}

//----------------------------------------------------------------------------

GLuint
LoadShaders(ShaderInfo* shaders)
{
    if (shaders == NULL) { return 0; }

    GLuint program = glCreateProgram();

    ShaderInfo* entry = shaders;
    while (entry->type != GL_NONE) {
        GLuint shader = glCreateShader(entry->type);

        entry->shader = shader;

        const GLchar* source = ReadShader(entry->filename);
        if (source == NULL) {
            for (entry = shaders; entry->type != GL_NONE; ++entry) {
                glDeleteShader(entry->shader);
                entry->shader = 0;
            }
            return 0;
        }

        glShaderSource(shader, 1, &source, NULL);
        
        // Liberar la memoria asignada en ReadShader
        free((void*)source);

        glCompileShader(shader);

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

            GLchar* log = (GLchar*)malloc(len + 1);
            if (log) {
                glGetShaderInfoLog(shader, len, &len, log);
                fprintf(stderr, "Shader compilation failed: %s\n", log);
                free(log);
            }
#endif /* DEBUG */
            return 0;
        }

        glAttachShader(program, shader);
        
        ++entry;
    }

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

        GLchar* log = (GLchar*)malloc(len + 1);
        if (log) {
            glGetProgramInfoLog(program, len, &len, log);
            fprintf(stderr, "Shader linking failed: %s\n", log);
            free(log);
        }
#endif /* DEBUG */

        for (entry = shaders; entry->type != GL_NONE; ++entry) {
            glDeleteShader(entry->shader);
            entry->shader = 0;
        }
        
        return 0;
    }

    return program;
}
