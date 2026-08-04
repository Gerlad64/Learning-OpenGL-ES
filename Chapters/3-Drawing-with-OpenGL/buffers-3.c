/**************************************************************
 PROGRAMA BASADO EN `buffers.c` QUE CREA UN BUFFER DE POSICIONES Y COLORES PARA DIBUJAR
 UN CUADRADO. LUEGO, LEE DE ESE BUFFER EN OTRA PARTE DEL CÓDIGO Y LO IMPRIME EN LA TERMINAL.

MAPEA EL BUFFER A UN PUNTERO EN LA APLICACIÓN, POR LO QUE EVITA COPIAR MEMORIA 
A LA APLICACIÓN O A OTRO BUFFER COMO EN LOS EJEMPLOS DE `buffers-2.c`

ESTE EJEMPLO UTILIZA LAS SIGUIENTES FUNCIONES DE OPENGL:

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
	

    	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(
		GL_ARRAY_BUFFER, //target a usar
		sizeof(positions) + sizeof(colors), // tamaño total del buffer
		NULL, // se inicializa sin datos (se llena con datos más abajo)
		GL_DYNAMIC_DRAW //flag para poder llenarlo después
	);
	
	/* ----- COLOCAR POSICIONES Y COLORES EN EL BUFFER ----- */
	
	glBufferSubData(
		GL_ARRAY_BUFFER, 	// target
		0,			//offset
		sizeof(positions), 	// tamaño
		positions		// data
	);
	
	glBufferSubData(
		GL_ARRAY_BUFFER, 	// target
		sizeof(positions),	//offset
		sizeof(colors), 	// tamaño
		colors			// data
	);
}

void read_buffers_from_ptr_old() {

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	GLfloat* data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	
	if (data) {
		GLfloat val;
		printf("-----Posiciones (glMapBuffer)-------\n");
		for(int i=0; i<16; i++) {
			val = data[i];
			printf(
				"%s%.1f, ", 
				val > 0 ? " " : "",
				val
			);
			if( i%4 == 3 )
				printf("\n");
		}
		printf("-----Colores (glMapBuffer)-------\n");
		for(int i = 16; i<16+12; i++) {
			val = data[i];
			printf("%.1f, ", val);
			if( (i-16)%3 == 2 )
				printf("\n");

		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	else
		printf("Error, no se pudo leer del buffer\n");

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void read_buffers_from_ptr() {
	printf("glMapNamedBufferRange no existe en esta versión (OpenGL 4.1)\n");
/**
	GLfloat* data = glMapNamedBufferRange(
		Buffers[ArrayBuffer],
		0,	
		sizeof(positions),
		GL_MAP_READ_BIT
	);
	if (data) {
		GLfloat val;
		printf("-----Posiciones (glMapNamedBufferRange)-------\n");
		for(int i=0; i<16; i++) {
			val = data[i];
			printf(
				"%s%.1f, ", 
				val > 0 ? " " : "",
				val
			);
			if( i%4 == 3 )
				printf("\n");
		}
		glUnmapNamedBuffer(Buffers[ArrayBuffer]);
	}
	else {
		printf("No se pudo leer del buffer");
	}
*/
}

void init_vertices() {
	// Crear y enlazar VAO
	glGenVertexArrays(NumVAOs, VAOs); 
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
		vColor,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		BUFFER_OFFSET(sizeof(positions))
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
	read_buffers_from_ptr_old();
	read_buffers_from_ptr();
}

void display() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);

	glBindVertexArray(VAOs[Square]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, NumVertices);
}


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // Inicializa glfw

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
