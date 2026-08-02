

#include <math.h>



typedef struct {
	float *positions;
	float *normals;
	unsigned short *indices;
} Shape;

#define SPHERE_VERTEX_COUNT(stackCount, vertexCount) \
	(((stackCount) + 1) * ((vertexCount) + 1))

#define SPHERE_INDEX_COUNT(stackCount, vertexCount) \
	(6 * (stackCount) * (vertexCount))


/**	Llena los datos de posición, normales e índices de una esfera 
	en una variable Shape de destino.
	
	@param[Shape] dest Estructura de destino
	@param[float] radius Radio de la esfera
	@param[int] stackCount Cantidad de *stacks* (paralelos o anillos)
	@param[int] sectorCount Cantidad de *sectors* (sectores o meridianos)

	@note Los tres punteros de la figura requieren memoria contigua (por separado),
	      en particular,
	      -		positions[3 * (stackCount + 1) * (sectorCount + 1)]
	      -		normals  [3 * (stackCount + 1) * (sectorCount + 1)]
	      -		indices  [6 * stackCount * sectorCount]

	@note La implementación y términos usados se basan en la información de la página:
	      *		https://www.songho.ca/opengl/gl_sphere.html
*/
void to_sphere(Shape dest, float radius, int stackCount, int sectorCount);
