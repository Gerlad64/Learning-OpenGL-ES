/**************************************************************
 PROGRAMA QUE CREA UN BUFFER DE POSICIONES Y COLORES PARA DIBUJAR
 UN CUADRADO

ESTE EJEMPLO UTILIZA LAS SIGUIENTES FUNCIONES DE OPENGL:

 Funciones de Buffer:
*	glCreateBuffers		: Crea nombres para acceder a los buffers
*	glNamedBufferStorage	: Reserva espacio en memoria a un buffer, y
				  opcionalmente lo inicializa con datos si se
				  le provee un puntero con datos.
*	glNamedBufferSubData	: Llena segmentos de un buffer con datos a partir
				  de un puntero.
 Funciones de Vertices:
*	glCreateVertexArrays	: Crea nombres para acceder a los Vertex Objects.
*	glVertexAttribPointer	: Especifica el tipo de dato (float, int, etc)
				  del contenido de un segmento del buffer.
				  Antes de usar esta función se debe enlazar
				  un buffer con glBindBuffer.

***************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(offset)((void *)(offset))


enum VAO_IDs { Square, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vColor = 1};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

constexpr GLuint NumVertices = 4;


static const GLfloat positions[] = {
	-1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f,-1.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 0.0f, 1.0f,
	 1.0f,-1.0f, 0.0f, 1.0f,
};
static const GLfloat colors[] = {
	 1.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
};


void init_buffers() {
	

    	glCreateBuffers(NumBuffers, Buffers);
	glNamedBufferStorage(
		Buffers[ArrayBuffer], //buffer a usar
		sizeof(positions) + sizeof(colors), // tamaño total del buffer
		NULL, // se inicializa sin datos (se llena con datos más abajo)
		GL_DYNAMIC_STORAGE_BIT //flag para poder llenarlo después
	);
	
	/* ----- COLOCAR POSICIONES Y COLORES EN EL BUFFER ----- */
	
	glNamedBufferSubData(
		Buffers[ArrayBuffer], 	// data
		0,			//offset
		sizeof(positions), 	// tamaño
		positions		// data
	);
	
	glNamedBufferSubData(
		Buffers[ArrayBuffer], 	// data
		sizeof(positions),	//offset
		sizeof(colors), 	// tamaño
		colors			// data
	);
}

void init_vertices() {
	// Crear y enlazar VAO
	glCreateVertexArrays(NumVAOs, VAOs); 
    	glBindVertexArray(VAOs[Square]);
	// Enlazar el buffer para que glVertexAttribPointer lo use
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);

	glVertexAttribPointer(
        	vPosition, 		// atributo
		4, 			// Cantidad de componentes (vec4)
		GL_FLOAT,		// tipo de dato
		GL_FALSE,		// normalizado
		0,			// stride
		BUFFER_OFFSET(0)	//offset
    	);

	glVertexAttribPointer(
		vColor,			// atributo
		3,			// Cantidad de componentes (vec3)
		GL_FLOAT,		// tipo de dato
		GL_FALSE,		// normalizado
		0,			// stride
		BUFFER_OFFSET(sizeof(positions))//offset
	);
    
	glEnableVertexAttribArray(vPosition);
	glEnableVertexAttribArray(vColor);

	glBindVertexArray(0);
}

void init() {
	ShaderInfo shaders[] = 
	{
		{ GL_VERTEX_SHADER,   "Chapters/3-Drawing-with-OpenGL/buffers.vert" },
		{ GL_FRAGMENT_SHADER, "Chapters/3-Drawing-with-OpenGL/buffers.frag" },
		{ GL_NONE, NULL },
	};
	GLuint program = LoadShaders(shaders); 
	glUseProgram(program);

	init_buffers();
	init_vertices();
}

void display() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);

	glBindVertexArray(VAOs[Square]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, NumVertices);
}


int main() {
	glfwInit(); // Inicializa glfw

	GLFWwindow* window = glfwCreateWindow(640, 480, "My First Square", NULL, NULL);
	glfwMakeContextCurrent(window);
	gl3wInit();

	init();

	while ( !glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window); //redibuja los contenidos en pantalla
		glfwPollEvents(); //revisa mensajes del SO
	}
	glfwDestroyWindow(window);
	
	glfwTerminate();

	return 0;
}
