/**************************************************************
 PROGRAMA BASADO EN LOS EJEMPLOS 3.5 Y 3.6 DEL LIBRO.

DIBUJA 4 TRIÁNGULOS, CADA UNO CON UN MÉTODO DE DIBUJO DISTINTO.

ESTE EJEMPLO UTILIZA LAS SIGUIENTES FUNCIONES DE OPENGL:
*	glUniformMatrix4fv: para pasarle una matriz de 4x4 al shader

  COMANDOS DE DIBUJO
*	glDrawArrays		: Dibuja primitivas usando elementos de un buffer y
				  en el orden en el que se encuentran.
*	glDrawElements		: Dibuja primitivas usando elementos de un buffer y
				  usando índices de otro buffer.
*	glDrawElementsBaseVertex: Igual a glDrawElements, pero a los indices se les suma
				  *baseVertex*. Útil para escribir índices de cada figura
				  de forma local (0, 1, 2...) y luego sumar *baseVertex*
				  para obtener el índice dentro del EBO (element buffer
				  object, buffer de índices).
*	glDrawArraysInstanced	: Cómo glDrawArrays con la capacidad de dibujar multiples 
				  instancias de una primitiva.

ADICIONALMENTE, EL EJEMPLO INTRODUCE LA MATRIZ DE MODELO (TRANSFORMACIONES) Y LA MATRIZ DE PERSPECTIVA, DONDE PARA OBTENER UNA INSTANCIA DE 
ESTAS SE LLAMAN A LAS FUNCIONES `transformation_mat` y `projection_mat` DE LA LIBRERÍA <MATH/matrices.h>.

PARA ENTENDER CÓMO SE DEDUCE LA MATRIZ DE PROYECCIÓN SE RECOMIENDAN
ESTOS VIDEOS:
*	https://youtu.be/LhQ85bPCAJ8?list=PLA0dXqQjCx0S04ntJKUftl6OaOgsiwHjA
*	https://youtu.be/md3jFANT3UM?list=PLA0dXqQjCx0S04ntJKUftl6OaOgsiwHjA

***************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <MATH/matrices.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(offset)((void *)(offset))


enum VAO_IDs { Triangles, NumVAOs };
/*
  vbo (Vertex Buffer Object): Buffer de siempre, para almacenar posiciones, colores, etc.
  ebo (ElementBufferObject) : Buffer para almacenar índices de vértices.
*/
enum Buffer_IDs { vbo, ebo, NumBuffers };
enum Attrib_IDs { vPosition = 0, vColor = 1};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint render_model_matrix_loc; // uniform location de model_matrix en el shader
GLuint render_proj_matrix_loc; // uniform location de projection_matrix en el shader


static const GLfloat positions[] = {
	-1.0f,-1.0f, 0.0f, 1.0f,
	 1.0f,-1.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f,-1.0f, 0.0f, 1.0f,
};
static const GLfloat colors[] = {
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 1.0f,
};

static const GLushort indices[] = {
	0, 1, 2
};

void init_buffers() {
		
    	glCreateBuffers(NumBuffers, Buffers);
 
	/**----------- Inicializar vbo --------*/
	glNamedBufferStorage(
		Buffers[vbo], //buffer a usar
		sizeof(positions) + sizeof(colors), // tamaño total del buffer
		NULL, // se inicializa sin datos (se llena con datos más abajo)
		GL_DYNAMIC_STORAGE_BIT
	);
	
	/* ----- COLOCAR POSICIONES Y COLORES EN EL BUFFER ----- */
	
	glNamedBufferSubData(
		Buffers[vbo], 		// data
		0,			//offset
		sizeof(positions), 	// tamaño
		positions		// data
	);
	
	glNamedBufferSubData(
		Buffers[vbo], 		// data
		sizeof(positions),	//offset
		sizeof(colors), 	// tamaño
		colors			// data
	);

	/**----------- Inicializar ebo --------*/
	glNamedBufferStorage(
		Buffers[ebo],		// buffer
		sizeof(indices),	// tamaño
		indices,		// data
		0			// flags (0 --> buffer inmutable)
	);
}


void init_vertices() {

	/**----------- Inicializar Vertices --------*/
	/** (No cambia con respecto a ejemplos anteriores) */
 
	glCreateVertexArrays(NumVAOs, VAOs); 
    	glBindVertexArray(VAOs[Triangles]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[vbo]);

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

void init_uniform_locations(GLuint program) {
	// location de la matriz de modelo
	render_model_matrix_loc = glGetUniformLocation(program, "model_matrix");
	// location de la matriz de proyección en perspectiva
	render_proj_matrix_loc = glGetUniformLocation(program, "projection_matrix");
}

void init() {
	ShaderInfo shaders[] = 
	{
		{ GL_VERTEX_SHADER,   "Chapters/3-Drawing-with-OpenGL/drawing.vert" },
		{ GL_FRAGMENT_SHADER, "Chapters/3-Drawing-with-OpenGL/drawing.frag" },
		{ GL_NONE, NULL },
	};
	GLuint program = LoadShaders(shaders); 
	glUseProgram(program);

	init_buffers();
	init_vertices();
	init_uniform_locations(program);

	mat4 proj_matrix;
	// se usa matriz de proyección en perspectiva
	// como el ejemplo del libro
	perspective_mat(
		proj_matrix, 	// dest  : matriz de destino
		90.0f,		// fovy  : angulo de visión en y
		640.0f / 480.0f,// aspect: relación de aspecto
		0.1f, 		// near  : distancia del plano near
		100.0f		// far	 : distancia del plano far
	);
    	glUniformMatrix4fv(render_proj_matrix_loc, 1, GL_FALSE, proj_matrix);
}

void draw_triangles() {

    	glBindVertexArray(VAOs[Triangles]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[ebo]);

	/* 
	  Se define una matriz de 4x4 para llamar a las diferentes 
	  funciones de dibujo
	*/
	
	static mat4 model_matrix;

	translation_mat(model_matrix, -3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	translation_mat(model_matrix, -1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

	translation_mat(model_matrix, 1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);
	
	translation_mat(model_matrix, 3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
}

void display() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 0, black);
 
	draw_triangles();
}


int main() {
	glfwInit(); // Inicializa glfw

	GLFWwindow* window = glfwCreateWindow(640, 480, "Drawing Commands", NULL, NULL);
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
