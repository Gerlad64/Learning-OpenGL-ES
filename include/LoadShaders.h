//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.h ---
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __LOAD_SHADERS_H__
#define __LOAD_SHADERS_H__

#include <GL/gl.h> 
// Nota: Dependiendo de tu cargador (GLEW, GLAD, GL3W), 
// tal vez necesites incluir ese header aquí en lugar de GL/gl.h

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Estructura compatible con C
typedef struct {
    GLenum       type;
    const char* filename;
    GLuint       shader;
} ShaderInfo;

GLuint LoadShaders(ShaderInfo* shaders);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __LOAD_SHADERS_H__
