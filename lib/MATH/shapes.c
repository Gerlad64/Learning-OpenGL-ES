

#include <MATH/shapes.h>

void to_sphere(Shape dest, float radius, int stackCount, int sectorCount) {
	float  x, y, z;
	float nx,ny,nz, radiusInv = 1.0f / radius;
	float phi, theta, r;
	float stackStep  = M_PI / stackCount;
	float sectorStep = 2.0f * M_PI / sectorCount;
	/* Posiciones y Normales */
	for(int index = 0, stack = 0; stack <= stackCount; stack++)  {
		phi = M_PI * -0.5f + (float)stack * stackStep;
		r = radius * cosf(phi);
		z = radius * sinf(phi);
		
		for( int sector = 0; sector <= sectorCount; sector++) {
			theta = (float)sector * sectorStep;
			x = r * cosf(theta);
			y = r * sinf(theta);
			
			nx = x * radiusInv;
			ny = y * radiusInv;
			nz = z * radiusInv;

			dest.positions[index]   = x;
			dest.positions[index+1] = y;
			dest.positions[index+2] = z;

			dest.normals[index]     = nx;
			dest.normals[index+1]   = ny;
			dest.normals[index+2]   = nz;
 
			index +=3;
		}

	}
	/* Índices */
	/* 
	*	El algoritmo llena los índices empezando por los stacks
	*	de abajo. Cuando termina, reinicia k1 y k2 para avanzar 
	*	al siguiente sector y volver a llenar los stacks.
				k2*-------*k2+1
				  |    /  Î
				  |  /    |
				k1*------>*k1+1
	*	Para avanzar al stack de arriba (o al siguiente sector), 
	*	se suma (sectorCount + 1).
	*	Para avanzar al stack de la derecha (el siguiente stack), 	
	*	se suma 1
	*	
	*/
	for(int k1, k2, index = 0, stack = 0; stack < stackCount; stack++) {
		k1 = stack * (sectorCount + 1);
		k2 = k1 + sectorCount + 1; // (stack + 1) * (sectorCount + 1)
		for( int sector = 0; sector < sectorCount; sector++,k1++,k2++) {
			// En stack == 0, no existe el vertice k1+1
			// debido a que se encuentra el polo sur donde hay
			// un solo punto
			if(stack != 0 ) {
				dest.indices[index++] = k1; 
				dest.indices[index++] = k1+1;
				dest.indices[index++] = k2+1;
			}
			// De manera similar, en este stack (polo norte)
			// no existe el vertice k2+1
			if(stack != (stackCount - 1)) {
				dest.indices[index++] = k2+1;
				dest.indices[index++] = k2;
				dest.indices[index++] = k1;

			}
		}
	}
}

void to_torus(Shape dest, float majorRadius, float minorRadius, int sideCount, int sectorCount) {
	float  x,  y,  z;
	float nx, ny, nz, inv_r = 1.0f / minorRadius;
	float phi, theta, r;
	float sideStep   = 2 * M_PI / sideCount; // de pi a -pi
	float sectorStep = 2* M_PI /sectorCount; // de 0 a 2pi

	for(int side = 0, index = 0; side <= sideCount; side++) {
		phi = M_PI - side * sideStep;
		r = minorRadius * cosf(phi);
		z = minorRadius * sinf(phi);
		
		for(int sector = 0; sector <= sectorCount; sector++) {
			theta = sector * sectorStep;
			x = r * cosf(theta);
			y = r * sinf(theta);
			
			nx = x * inv_r;
			ny = y * inv_r;
			nz = z * inv_r;

			x += majorRadius * cosf(theta);
			y += majorRadius * sinf(theta);

			dest.positions[index]   = x;
			dest.positions[index+1] = y;
			dest.positions[index+2] = z;

			dest.normals[index]     = nx;
			dest.normals[index+1]   = ny;
			dest.normals[index+2]   = nz;
 
			index +=3;
		}

	}

	for(int k1, k2, index = 0, side = 0; side < sideCount; side++) {

		k1 = side * (sectorCount + 1);
		k2 = k1 + sectorCount + 1; // (stack + 1) * (sectorCount + 1)

		for( int sector = 0; sector < sectorCount; sector++,k1++,k2++) {

			dest.indices[index++] = k1; 
			dest.indices[index++] = k2+1;
			dest.indices[index++] = k1+1;

			dest.indices[index++] = k1;
			dest.indices[index++] = k2;
			dest.indices[index++] = k2+1;
			
		}
	}

}

