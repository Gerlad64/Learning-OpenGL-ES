#ifndef __MATH_MATRICES_H__
#define __MATH_MATRICES_H__

#include <math.h>

typedef float mat4[16];

static inline void translation_mat(mat4 dest, float x, float y, float z) {
	dest[0] = 1; dest[4] = 0; dest[8] = 0;  dest[12] = x;
    	dest[1] = 0; dest[5] = 1; dest[9] = 0;  dest[13] = y;
    	dest[2] = 0; dest[6] = 0; dest[10] = 1; dest[14] = z;
    	dest[3] = 0; dest[7] = 0; dest[11] = 0; dest[15] = 1;
}

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

#endif
