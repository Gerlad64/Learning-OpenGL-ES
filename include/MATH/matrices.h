#ifndef __MATH_MATRICES_H__
#define __MATH_MATRICES_H__

#include <math.h>

typedef float mat4[16];


/** ------- OPERACIONES ---------*/

/** Multiplica dos matrices (4x4) y guarda el resultado en la matriz de destino.
 *
 *  @param[mat4] dest Matriz de destino
 *  @param[mat4] a Matriz a la izquierda de la multiplicación
 *  @param[mat4] b Matriz a la derecha de la multiplicación
 *
 *  @note Esta operación sobreescribe los valores guardados en la matriz de destino
 *  @TODO Implementar usando SIMD
*/
static inline void mat4_mul(mat4 dest, const mat4 a, const mat4 b) {
	mat4 temp;
	for (int i = 0; i < 4; i++) {
		// Extrae la columna i de la matriz B
		float b0 = b[i * 4 + 0];
		float b1 = b[i * 4 + 1];
		float b2 = b[i * 4 + 2];
		float b3 = b[i * 4 + 3];

		// Multiplica usando las filas de A
		temp[i * 4 + 0] = a[0] * b0 + a[4] * b1 + a[8]  * b2 + a[12] * b3;
		temp[i * 4 + 1] = a[1] * b0 + a[5] * b1 + a[9]  * b2 + a[13] * b3;
		temp[i * 4 + 2] = a[2] * b0 + a[6] * b1 + a[10] * b2 + a[14] * b3;
		temp[i * 4 + 3] = a[3] * b0 + a[7] * b1 + a[11] * b2 + a[15] * b3;
	}
	
	for (int i = 0; i < 16; i++) {
		dest[i] = temp[i];
	}
}


/** ----- TRANSFORMACIONES ----- */

#define BEGIN_TRANSFORM(dest_mat) \
    { \
        float* _tr_dest = (dest_mat); \
        mat4 _s; \
        mat4_identity(_tr_dest);

#define TRANSFORM(func, ...) \
    { \
        func(_s, __VA_ARGS__); \
        mat4_mul(_tr_dest, _tr_dest, _s); \
    }

#define END_TRANSFORM() }


static inline void mat4_identity(mat4 dest) {
	for (int i = 0; i <16; i++)
		dest[i] = 0.0f;
	dest[0]  = 1.0f;
	dest[5]  = 1.0f;
	dest[10] = 1.0f;
	dest[15] = 1.0f;
}

/** Convierte la matriz de destino (4x4) en una matriz de traslación.
 *  
 * 
 *  @param[mat4] dest Matriz de destino
 *  @param[float] x traslación en el eje X
 *  @param[float] y traslación en el eje Y
 *  @param[float] z traslación en el eje Z
 *
 *  @note Esta operación sobreescribe los valores guardados en la matriz.
*/
static inline void translation_mat(mat4 dest, float x, float y, float z) {
	dest[0] = 1; dest[4] = 0; dest[8] = 0;  dest[12] = x;
    	dest[1] = 0; dest[5] = 1; dest[9] = 0;  dest[13] = y;
    	dest[2] = 0; dest[6] = 0; dest[10] = 1; dest[14] = z;
    	dest[3] = 0; dest[7] = 0; dest[11] = 0; dest[15] = 1;
}

static inline void frustum_mat(
    mat4 dest,
    float left,
    float right,
    float bottom,
    float top,
    float near_plane,
    float far_plane
) {
    // Llenar todo con ceros primero
    for(int i = 0; i < 16; i++) dest[i] = 0.0f;

    // Si los parámetros son invalidos se pasa la identidad.
    if ((right == left) ||
        (top == bottom) ||
        (near_plane == far_plane) ||
        (near_plane < 0.0f) ||
        (far_plane < 0.0f)) {
        
        dest[0] = 1.0f;
        dest[5] = 1.0f;
        dest[10] = 1.0f;
        dest[15] = 1.0f;
        return;
    }

    dest[0]  = (2.0f * near_plane) / (right - left);
    dest[5]  = (2.0f * near_plane) / (top - bottom);
    dest[8]  = (right + left) / (right - left);
    dest[9]  = (top + bottom) / (top - bottom);
    dest[10] = -(far_plane + near_plane) / (far_plane - near_plane);
    dest[11] = -1.0f;
    dest[14] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
}

/** Convierte la matriz de destino (4x4) en una matriz de proyección en perspectiva.
 * 
 *  @param[mat4] dest Matriz de destino
 *  @param[float] fovy_deg Ángulo de visión (en grados)
 *  @param[float] aspect Relación de aspecto (width/height)
 *  @param[float] near_plane Distancia del *near_plane*
 *  @param[float] far_plane Distancia del *far_plane*
 *
 *  @note Esta operación sobreescribe los valores guardados en la matriz.
*/
static inline void perspective_mat(
	mat4 dest,
	float fovy_deg,
	float aspect,
	float near_plane,
	float far_plane
) {
    float f = 1.0f / tanf(fovy_deg * (3.1415926535f / 360.0f));
    
    // Llenar todo con ceros primero
    for(int i = 0; i < 16; i++) dest[i] = 0.0f;
    
    dest[0] = f / aspect;
    dest[5] = f;
    dest[10] = (far_plane + near_plane) / (near_plane - far_plane);
    dest[11] = -1.0f;
    dest[14] = (2.0f * far_plane * near_plane) / (near_plane - far_plane);
}

/** Convierte la matriz de destino (4x4) en una matriz de rotación con respecto a un eje arbitrario.
 * 
 *  @param[mat4] dest Matriz de destino
 *  @param[float] angle Ángulo de la rotación
 *  @param[float] x Coordenada X del eje de rotación
 *  @param[float] y Coordenada Y del eje de rotación
 *  @param[float] z Coordenada Z del eje de rotación
 *
 *  @note Esta operación sobreescribe los valores guardados en la matriz.
*/
static inline void rotation_mat(mat4 dest, float angle, float x, float y, float z)
{
    float len = sqrtf(x * x + y * y + z * z);
    if (len != 0.0f)
    {
        x /= len;
        y /= len;
        z /= len;
    }

    const float rads = angle * 0.0174532925f; // PI / 180
    const float c = cosf(rads);
    const float s = sinf(rads);
    const float omc = 1.0f - c;

    const float x2 = x * x;
    const float y2 = y * y;
    const float z2 = z * z;

    dest[0] =    x2 * omc + c    ; dest[4] = x * y * omc - z * s;
    dest[1] = x * y * omc + z * s; dest[5] =    y2 * omc + c    ;
    dest[2] = x * z * omc - y * s; dest[6] = y * z * omc + x * s;
    dest[3] =        0.0f        ; dest[7] =        0.0f        ;

    
    dest[8] = x * z * omc + y * s; dest[12]= 0.0f;
    dest[9] = y * z * omc - x * s; dest[13]= 0.0f;
    dest[10]=    z2 * omc + c    ; dest[14]= 0.0f;
    dest[11]=        0.0f        ; dest[15]= 1.0f;
}

#endif
