/**************************************************************
 PROGRAMA QUE DIBUJA 1 PUNTO QUE OSCILA DE ARRIBA A ABAJO
 USANDO EL VERTEX PROGRAM/SHADER.

 ESTE CÓDIGO SE BASA EN `triangles.c` PERO CON PEQUEÑOS
 AJUSTES PARA DIBUJAR UN PUNTO EN VEZ DE TRIANGULOS.
 LO MÁS LLAMATIVO ES EL USO DE LAS FUNCIONES
	glGetUniformLocation
	glUniform*
 USADAS PARA PASAR ESTABLECER EL VALOR DE UNA VARIABLE
 `uniform` DEL VERTEX PROGRAM.

	FLAGS DE COMPILACIÓN NECESARIAS: 
	-lGL -ldl -lglfw

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
GLint timeLoc; // indice "time" en el vertex shader

/* 
 FUNCIÓN A SER EJECUTADA EN CADA CICLO.
 EN CADA FRAMES ACTUALIZA LA VARIABLE 
 `time` DEL VERTEX SHADER.
 
*/
void pass_time() {
	static GLfloat timeVal; // paso del tiempo normalizado [0, 1]
	static int64_t current_nsec;  // tiempo actual en nanosegundos
	static int64_t past_nsec = 0; // tiempo pasado en nano segundos
	static int64_t accumulator = 0; // acumulador de tiempo
	constexpr int64_t FRAMES_NSEC = 1000000000LL / 120; // 120 FPS
	constexpr int64_t PERIOD = 1<<31; // 2^29 <~ 1.000.000.000

	static struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	// suma segundos mas nano-segundos en unidades de nano-segundos
	current_nsec= (int64_t)t.tv_sec * 1000000000LL + (int64_t)t.tv_nsec;

	if( past_nsec == 0 ) {
		past_nsec = current_nsec;
		return;
	}
	accumulator += current_nsec - past_nsec;
	past_nsec = current_nsec;
	
	if (accumulator >= FRAMES_NSEC) {
		// timeVal == (current mod PERIOD) / PERIOD
		timeVal = (GLfloat)((current_nsec  & (PERIOD-1)) / (float)PERIOD);
		// actualiza `uniform time`
		glUniform1f(timeLoc, timeVal);
		accumulator -= FRAMES_NSEC;
	}
	return;
}

void init() {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER,   "Chapters/2-Shaders/time.vert" },
		{ GL_FRAGMENT_SHADER, "Chapters/2-Shaders/time.frag" },
		{ GL_NONE, NULL },
	};
	program = LoadShaders(shaders);
	glUseProgram(program);
	
	/** obtiene el índice de la variable time en el vertex program*/
	timeLoc = glGetUniformLocation(program, "time");

	glBindVertexArray(VAOs[Points]);

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

void display() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);

	glBindVertexArray(VAOs[Points]);
	glDrawArrays(GL_POINTS, 0, NumVertices);
}


int main() {
	glfwInit(); // Inicializa glfw

	// configurar versión
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// configura una ventana
	GLFWwindow* window = glfwCreateWindow(640, 480, "Oscilating Dot", NULL, NULL); 
	// configura `window como el contexto actual`
	glfwMakeContextCurrent(window); 
	gl3wInit();

	init();

	while ( !glfwWindowShouldClose(window)) {
		pass_time(); // avanza el tiempo
		display();
        	glfwSwapBuffers(window); //redibuja los contenidos en pantalla
        	glfwPollEvents(); //revisa mensajes del SO
    	}
    	glfwDestroyWindow(window);
    
    	glfwTerminate();

    return 0;
}
