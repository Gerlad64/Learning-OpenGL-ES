/**************************************************************
PROGRAMA QUE DIBUJA UN TORO, UTILIZADO MÁS ADELANTE EN EL PROGRAMA `stencil`

ESTE EJEMPLO UTILIZA FUNCIONES YA ESTUDIADAS ANTES, A EXCEPCIÓN DE: 
*	`glDisableVertexAttribArray`
*	`glVertexAttrib3f`
*	`glVertexAttrib4f` 

AUNQUE SU USO ES SIMPLE, MUESTRA UN  USO PRÁCTICO DE ESTAS.

PARA CREAR LA ESFERA, SE INTRODUCE LA LIBRARÍA <MATH/shapes.h>

***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <MATH/matrices.h>
#include <MATH/shapes.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(offset)((void *)(offset))


enum VAO_IDs { vao, NumVAOs };
/*
  vbo (Vertex Buffer Object): Buffer de siempre, para almacenar posiciones, colores, etc.
  ebo (ElementBufferObject) : Buffer para almacenar índices de vértices.
*/
enum Buffer_IDs { vbo, ebo, model_matrix_buffer, NumBuffers };
enum Attrib_IDs { 
	position_loc = 0,
	normal_loc = 1,
	color_loc = 2,
	matrix_loc = 3
};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

GLuint render_view_matrix_loc;
GLuint render_proj_matrix_loc;

/* posiciones y normales de un cubo (sin índices) */


constexpr int q = 32; // quality

// cantidad de vertices
constexpr int vertex_count = TORUS_VERTEX_COUNT(q, q);
// cantidad de componentes de vertices (positions_len == normals_len)
constexpr int vertex_component_count = 3 * TORUS_VERTEX_COUNT(q,q);
// tamaño en bytes que ocupa vertex_component_count
constexpr int vertex_component_size = sizeof(float) * vertex_component_count;
// cantidad de datos (posicions_len + normals_len)
constexpr int vertex_data_count      = 2 * vertex_component_count;
// tamaño en bytes de vertex_data_count
constexpr int vertex_buffer_size     = sizeof(float) * vertex_data_count; 

constexpr int index_count = TORUS_INDEX_COUNT(q,q);
constexpr int index_buffer_size = sizeof(unsigned short) * index_count;


void init_buffers() {
	// Se crea un solo arreglo con datos de vertices
	// y uno solo con datos de índices de modo que vivan
	// en el stack en el tiempo de vida de init_buffers
	GLfloat vertex_data[vertex_data_count];
	GLushort indices[index_count];

	Shape torus;
	torus.positions = vertex_data; 
	torus.normals   = vertex_data + vertex_component_count;
	torus.indices   = indices;

	to_torus(torus, 0.8f, 0.5f, q, q);

    	glCreateBuffers(NumBuffers, Buffers);
 
	/**----------- Inicializar vbo --------*/
	glNamedBufferStorage(
		Buffers[vbo], //buffer a usar
		vertex_buffer_size, // tamaño total del buffer
		vertex_data,//data
		0 //flags
	);

	/**----------- Inicializar ebo --------*/
	glNamedBufferStorage(
		Buffers[ebo],
		index_buffer_size,
		indices,
		0
	);
	
	
	/**----------- Inicializar model_matrix --------*/
	glNamedBufferStorage(
		Buffers[model_matrix_buffer],
		sizeof(mat4),
		NULL,
		GL_MAP_WRITE_BIT
	);
}


void init_vertices() {

	/**----------- Inicializar Vertices --------*/
	/** (No cambia con respecto a ejemplos anteriores) */
 
	glCreateVertexArrays(NumVAOs, VAOs); 
    	glBindVertexArray(VAOs[vao]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[vbo]);

	glVertexAttribPointer(
        	position_loc, 		// atributo
		3, 			// Cantidad de componentes (vec3)
		GL_FLOAT,		// tipo de dato
		GL_FALSE,		// normalizado
		0,			// stride
		BUFFER_OFFSET(0)	//offset
    	);
	
	glVertexAttribPointer(
		normal_loc,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		BUFFER_OFFSET(vertex_component_size)
	);

	glEnableVertexAttribArray(position_loc);
	glEnableVertexAttribArray(normal_loc);

	glDisableVertexAttribArray(color_loc);
	// establece color azulado para todos los vertices
	glVertexAttrib3f(color_loc, 0.0f, 0.364f, 0.636f);


	glBindBuffer(GL_ARRAY_BUFFER, Buffers[model_matrix_buffer]);
	// un mat4 toma 4 locations al ser 4 vec4
	// por lo que se llama a glVertexAttribPointer 4 veces
	for (int i = 0; i < 4; i++) { 
		glVertexAttribPointer(
			matrix_loc + i,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(mat4),
			BUFFER_OFFSET(4*sizeof(GL_FLOAT)*i)
		);
		glEnableVertexAttribArray(matrix_loc + i);
		glVertexAttribDivisor(matrix_loc+i, 1);
	}	
	glBindVertexArray(0);
}



void init_uniform_locations(GLuint program) {
	// location de la matriz de modelo
	render_view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	// location de la matriz de proyección en perspectiva
	render_proj_matrix_loc = glGetUniformLocation(program, "projection_matrix");
}


void init() {
	ShaderInfo shaders[] = 
	{
		{ GL_VERTEX_SHADER,   "Chapters/3-Drawing-with-OpenGL/instancing.vert" },
		{ GL_FRAGMENT_SHADER, "Chapters/3-Drawing-with-OpenGL/instancing.frag" },
		{ GL_NONE, NULL },
	};
	GLuint program = LoadShaders(shaders); 
	glUseProgram(program);

	init_buffers();
	init_vertices();
	init_uniform_locations(program);

	mat4 proj_matrix;
	// se usa matriz de frustum con los mismos
	// valores que en el repositorio de ejemplos
	frustum_mat(
		proj_matrix, 		// dest  : matriz de destino
		-1.0f,		
		 1.0f,
		-480.0f/640.0f,
		480.0f / 640.0f,
		1.0f, 			// near  : distancia del plano near
		5000.0f		// far	 : distancia del plano far
	);
    	glUniformMatrix4fv(render_proj_matrix_loc, 1, GL_FALSE, proj_matrix);
	mat4 view_matrix;
	
	translation_mat(
		view_matrix,
		0.0f,
		0.0f,
		-3.0f
	);
    	glUniformMatrix4fv(render_view_matrix_loc, 1, GL_FALSE, view_matrix);

	
}

void update(float t) {

	t *= 15;

	GLfloat* model_matrix = (GLfloat*)glMapNamedBufferRange(
		Buffers[model_matrix_buffer], 
		0,
		sizeof(mat4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
	);

	BEGIN_TRANSFORM(model_matrix)
		TRANSFORM(rotation_mat, 1.5f *  t, 1.0f, 0.0f, 0.0f)
		TRANSFORM(rotation_mat, 0.6f * t, 0.0f, 0.0f, 1.0f)
	END_TRANSFORM()

	glUnmapNamedBuffer(Buffers[model_matrix_buffer]);

}


void display() {
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const float one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, &one);
	
	// Descarta dibujar caras de polígonos
	// por defecto, las caras traceras (GL_BACK)
	glEnable(GL_CULL_FACE); 
	// Activa el Z-Buffer para que los objetos cercanos
	// tapen a los objetos lejanos cuando están detrás
	glEnable(GL_DEPTH_TEST);
	// Define la regla para que un fragmento pase el DEPTH_TEST
	glDepthFunc(GL_LEQUAL);


    	glBindVertexArray(VAOs[vao]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[ebo]);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, NULL);
}

void print_fps(float dt)
{
    static double elapsed = 0.0;
    static int frames = 0;

    elapsed += dt;
    frames++;

    if (elapsed >= 1.0) {
        printf("\rFPS: %3d", frames);
        fflush(stdout);

        elapsed -= 1.0;
        frames = 0;
    }
}

int main() {
	glfwInit(); // Inicializa glfw

	GLFWwindow* window = glfwCreateWindow(640, 480, "Instance Drawing", NULL, NULL);
	glfwMakeContextCurrent(window);
	gl3wInit();

	init();
	float app_time, last_time, dt;
	while ( !glfwWindowShouldClose(window)) {
		app_time = (float)glfwGetTime();
		dt = app_time-last_time;
		last_time=app_time;
		update(app_time);
		display();
		glfwSwapBuffers(window); //redibuja los contenidos en pantalla
		glfwPollEvents(); //revisa mensajes del SO
		print_fps(dt);
	}
	printf("\n");
	glfwDestroyWindow(window);
	
	glfwTerminate();

	return 0;
}
