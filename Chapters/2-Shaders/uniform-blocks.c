/**************************************************************
 PROGRAMA QUE DIBUJA 1 PUNTO EN UNA POSICIÓN PASADA AL SHADER
MEDIANTE UNIFORM-BLOCKS

 ESTE CÓDIGO SE BASA EN `uniform.c`, AÑADIENDO LA RUTINA
`init_uniform_blocks` E `init_point`. ESTA ÚLTIMA ENCAPSULA
LA LÓGICA DE CREAR EL PUNTO EN PANTALLA

ESTE EJEMPLO INTRODUCE LAS SIGUIENTES FUNCIONES DE OPENGL:
*	glBufferData
*	glGetUniformBlockIndex
*	glUniformBlockBinding
*	glBindBufferBase
*	glGetActiveUniformBlockiv --> solo se menciona
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(offset)((void *)(offset))

enum VAO_IDs { Points, NumVAOs }; 
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

constexpr GLuint NumVertices = 1;

GLuint program; // ahora se define acá


GLuint ubo; // uniform buffer object
GLuint uboIndex;
/* 
En este ejemplo no es necesario usar uboSize, porque la estructura 
guarda las variables en formato std140. 

De todos modos, es posible obtener el tamaño de la estructura
usando glGetActiveUniformBlockiv, que guarda el tamaño en uboSize. */

//GLuint uboSize;

// Estructura de datos de ejemplo para pasarselos al shader
typedef struct Uniforms {
	float translation_with_scale[4]; //xyz tralation w scale
	float rotation;
	int enabled;
	int padding[2];
} Uniforms;

void init_uniform_buffers(GLuint program) {
	/* Crear datos para pasarle al uniform block */
	Uniforms u = {0};
	u.translation_with_scale[0] = 0.5; // x
	u.translation_with_scale[1] = 0.5; // y
	u.translation_with_scale[2] = 0.0; // z
	u.translation_with_scale[3] = 45.2; // escala
	u.rotation = 1.4;
	u.enabled = 1;

	/**
	uboIndex = glGetUniformBlockIndex(program, "Uniforms");	
	glGetActiveUniformBlockiv(program, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);
	*/	

	/* 
		Este segmento es independiente del programa (GLuint program).
		Se puede hacer bind del buffer una sola vez y usarlo en distintos programas
	*/
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(
		GL_UNIFORM_BUFFER,
		sizeof(Uniforms),
		&u, 
		GL_STATIC_DRAW 
		// Si sus datos fuera a inicializados una vez
		// Se usaría GL_STATIC_DRAW
	);
	
	uboIndex = glGetUniformBlockIndex(program, "Uniforms");
	GLuint bindingPoint = 2; // puede ser cualquier numero, se usa 2 para ejemplificar
	glUniformBlockBinding(program, uboIndex, bindingPoint);

	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
}

void init_point(GLuint program) {
	// HABILITAR EL CAMBIO DE TAMAÑO DEL PUNTO
	// VISTO EN EL CAPÍTULO 3
	glEnable(GL_PROGRAM_POINT_SIZE);	

	/** esencialmente el mismo código que triangles */
	static const GLfloat vertices[NumVertices][2] = 
	{
		// mitad de la pantalla
		{ 0.0, 0.0 }, 
	};

	glGenVertexArrays(NumVAOs, VAOs); 
	glGenBuffers(NumBuffers, Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertices),
		vertices,
		GL_STATIC_DRAW
	);

	glBindVertexArray(VAOs[Points]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);

	glVertexAttribPointer(
		vPosition,		  
		2,		
		GL_FLOAT,
		GL_FALSE,
		0,	  
		BUFFER_OFFSET(0)
	);
	glEnableVertexAttribArray(vPosition);
}

void init() {
						

    	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER,   "Chapters/2-Shaders/uniform-blocks.vert" },
		{ GL_FRAGMENT_SHADER, "Chapters/2-Shaders/uniform-blocks.frag" },
		{ GL_NONE, NULL },
	};
	program = LoadShaders(shaders);
	glUseProgram(program);
	init_uniform_buffers(program);
	init_point(program);
}

void display() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);
	glBindVertexArray(VAOs[Points]);
	glDrawArrays(GL_POINTS, 0, NumVertices);
}


int main() {
	glfwInit(); // Inicializa glfw

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// configura una ventana
	GLFWwindow* window = glfwCreateWindow(640, 480, "Transformed Dot", NULL, NULL); 
	// configura `window como el contexto actual`
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
