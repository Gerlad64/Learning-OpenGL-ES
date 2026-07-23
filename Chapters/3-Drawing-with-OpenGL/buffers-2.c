/**************************************************************
 PROGRAMA BASADO EN `buffers.c`

ESTE EJEMPLO MUESTRA CÓMO LEER (O RECUPERAR) LOS DATOS DE UN BUFFER Y CÓMO COPIAR LOS
DATOS DE UN BUFFER A OTRO. ESTO SE HACE EN LAS FUNCIONES `read_buffers()` y `copy_buffers()`


ESTE EJEMPLO UTILIZA LAS SIGUIENTES FUNCIONES DE OPENGL:
*	glGetNamedBufferSubData: Leer datos
*	glCopyNamedBufferSubData: Copiar datos


LO RELEVANTE DE ESTE EJEMPLO ES ENTENDER QUE AMBAS FUNCIONES COPIAN MEMORIA,
YA SEA DE UN BUFFER A OTRO BUFFER O DE UN BUFFER A MEMORIA DE LA APLICACIÓN.
***************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(offset)((void *)(offset))


enum VAO_IDs { Square, NumVAOs }; 
enum Buffer_IDs { ArrayBuffer, CopyArrayBuffer, NumBuffers }; // --> se agrega un buffer copia
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
		Buffers[ArrayBuffer], 	// buffer
		0,			//offset
		sizeof(positions), 	// tamaño
		positions		// data
	);
	
	glNamedBufferSubData(
		Buffers[ArrayBuffer], 	// buffer
		sizeof(positions),	// offset
		sizeof(colors), 	// tamaño
		colors			// data
	);
}

void read_buffers() {

	// puntero donde se almacenarán los datos del buffer
	GLfloat* pos_data = malloc(sizeof(positions));
	// tambien puede definirse como:
	//GLfloat pos_data[sizeof(positions) / sizeof(positions[0])];
 
	/* 
	  Copiar datos del buffer (memoria de OpenGL) 
	  a pos_data (memoria de la aplicación)
	*/
	glGetNamedBufferSubData(
		Buffers[ArrayBuffer], 	// buffer
		0,			// offset
		sizeof(positions),	// tamaño
		pos_data		// data
	);

	/* Para mostrar que se leyó el buffer correctamente,
	   se imprimirán sus valores.
	*/
	printf("----Posiciones----\n");
 
	GLfloat pos;
	for(int i=0; i<16; i++) {
		pos = pos_data[i];
		printf(
			"%s%.1f, ", 
			pos > 0 ? " " : "", // espacio adicional si es mayor a 0
			pos
		);
		if( i%4 == 3 )
			printf("\n");
	}
	free(pos_data);
}


void copy_buffers() {

	/* Asignar espacio al buffer */
	glNamedBufferStorage(
		Buffers[CopyArrayBuffer],
		sizeof(colors),		
		NULL,
		GL_DYNAMIC_STORAGE_BIT
	);

	/* Copiar datos de ArrayBuffer a CopyArrayBuffer */
	glCopyNamedBufferSubData(
		Buffers[ArrayBuffer], 	  // readBuffer
		Buffers[CopyArrayBuffer], // writeBuffer
		sizeof(positions), 	  // readOffset
		0, 			  // writeOffset
		sizeof(colors) 		  // size
	);

	/** Leer datos del buffer copia e imprimirlos */

	GLfloat color_data[sizeof(colors) / sizeof(colors[0])];
	glGetNamedBufferSubData(
		Buffers[CopyArrayBuffer],
		0,
		sizeof(colors),
		color_data
	);

	printf("----Colores----\n");

	for(int i=0; i<12; i++) {
		printf("%.1f, ", color_data[i]);
		if( i%3 == 2 )
			printf("\n");
	}
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
	read_buffers();
	copy_buffers();
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
