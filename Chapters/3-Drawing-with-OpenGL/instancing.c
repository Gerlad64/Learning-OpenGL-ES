/**************************************************************
 PROGRAMA BASADO EN LOS EJEMPLOS 3.9 AL 3.12 DEL LIBRO Y 
 AL CÓDIGO 03-instancing3.cpp DEL REPOSITORIO DE EJEMPLOS DEL LIBRO:

* https://github.com/openglredbook/examples/blob/master/src/03-instancing3/03-instancing3.cpp

DIBUJA MÚLTIPLES CUBOS ROTANDO Y MOVIENDOSE, MIENTRAS LA CÁMARA TAMBIÉN LO HACE.

ESTE EJEMPLO MUESTRA CÓMO UTILIZAR INSTANCING PARA DIBUJAR MÚLTIPLES INSTANCIAS
DE UN MODELO CON POCAS LLAMADAS A LA API.

LO NOVEDOSO DE ESTE PROGRAMA ES LA INICIALIZACIÓN Y ACTUALIZACIÓN DE LAS MATRICES.
ADEMÁS, ESTA IMPLEMENTACIÓN DE INSTANCING PERMITE QUE EL SHADER NO DEPENDA DE LAS
INSTANCIAS, POR LO QUE SE PUEDE USAR CON OTROS OBJETOS QUE NO LO USEN.

ESTE EJEMPLO UTILIZA LAS SIGUIENTES FUNCIONES DE OPENGL:
*	glVertexAttribDivisor	: Permite especificar cada cuanto se consume
				  un elemento de un buffer o array especificado.
				  - 0 indica que se consume uno por vértice,
				  - 1 indica que se consume uno por instancia.
*	glDrawArraysInstanced	: Mostrada en un programa anterior

ESTAS SE LLAMAN A LAS FUNCIONES `transformation_mat` y `projection_mat` DE LA LIBRERÍA <MATH/matrices.h>.


***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <MATH/matrices.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(offset)((void *)(offset))


enum VAO_IDs { vao, NumVAOs };
/*
  vbo (Vertex Buffer Object): Buffer de siempre, para almacenar posiciones, colores, etc.
  ebo (ElementBufferObject) : Buffer para almacenar índices de vértices.
*/
enum Buffer_IDs { vbo, model_matrix_buffer, NumBuffers };
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

static const GLfloat positions[] = {
    // Frente (+Z)
    -30.0f, -30.0f,  30.0f,
     30.0f, -30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
    -30.0f, -30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
    -30.0f,  30.0f,  30.0f,

    // Atrás (-Z)
     30.0f, -30.0f, -30.0f,
    -30.0f, -30.0f, -30.0f,
    -30.0f,  30.0f, -30.0f,
     30.0f, -30.0f, -30.0f,
    -30.0f,  30.0f, -30.0f,
     30.0f,  30.0f, -30.0f,

    // Izquierda (-X)
    -30.0f, -30.0f, -30.0f,
    -30.0f, -30.0f,  30.0f,
    -30.0f,  30.0f,  30.0f,
    -30.0f, -30.0f, -30.0f,
    -30.0f,  30.0f,  30.0f,
    -30.0f,  30.0f, -30.0f,

    // Derecha (+X)
     30.0f, -30.0f,  30.0f,
     30.0f, -30.0f, -30.0f,
     30.0f,  30.0f, -30.0f,
     30.0f, -30.0f,  30.0f,
     30.0f,  30.0f, -30.0f,
     30.0f,  30.0f,  30.0f,

    // Arriba (+Y)
    -30.0f,  30.0f,  30.0f,
     30.0f,  30.0f,  30.0f,
     30.0f,  30.0f, -30.0f,
    -30.0f,  30.0f,  30.0f,
     30.0f,  30.0f, -30.0f,
    -30.0f,  30.0f, -30.0f,

    // Abajo (-Y)
    -30.0f, -30.0f, -30.0f,
     30.0f, -30.0f, -30.0f,
     30.0f, -30.0f,  30.0f,
    -30.0f, -30.0f, -30.0f,
     30.0f, -30.0f,  30.0f,
    -30.0f, -30.0f,  30.0f
};

static const GLfloat normals[] = {
    // Frente
     0, 0, 1,  0, 0, 1,  0, 0, 1,
     0, 0, 1,  0, 0, 1,  0, 0, 1,

    // Atrás
     0, 0,-1,  0, 0,-1,  0, 0,-1,
     0, 0,-1,  0, 0,-1,  0, 0,-1,

    // Izquierda
    -1, 0, 0, -1, 0, 0, -1, 0, 0,
    -1, 0, 0, -1, 0, 0, -1, 0, 0,

    // Derecha
     1, 0, 0,  1, 0, 0,  1, 0, 0,
     1, 0, 0,  1, 0, 0,  1, 0, 0,

    // Arriba
     0, 1, 0,  0, 1, 0,  0, 1, 0,
     0, 1, 0,  0, 1, 0,  0, 1, 0,

    // Abajo
     0,-1, 0,  0,-1, 0,  0,-1, 0,
     0,-1, 0,  0,-1, 0,  0,-1, 0
};

constexpr int INSTANCE_COUNT = 100;

// Se define un color para cada instancia
static GLfloat colors[INSTANCE_COUNT][3];

const size_t colors_offset = sizeof(positions) + sizeof(normals);
const size_t buffer_size = colors_offset + sizeof(colors);



void init_buffers() {

	// genera una curva de Lissajous con los colores
	// en función de su número de instancia
	// las componentes (r,g,b) oscilan entre 0.5 y 1.0
	for (int n=0; n < INSTANCE_COUNT; n++) {
		float a = n / 4.0f;
		float b = n / 5.0f;
		float c = n / 6.0f;

		colors[n][0] = 0.5f + 0.25f * (sinf(a + 1.0f) + 1.0f);
		colors[n][1] = 0.5f + 0.25f * (sinf(b + 2.0f) + 1.0f);
		colors[n][2] = 0.5f + 0.25f * (sinf(c + 3.0f) + 1.0f);
	}		

    	glCreateBuffers(NumBuffers, Buffers);
 
	/**----------- Inicializar vbo --------*/
	glNamedBufferStorage(
		Buffers[vbo], //buffer a usar
		buffer_size, // tamaño total del buffer
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
		sizeof(normals), 	// tamaño
		normals			// data
	);	

	glNamedBufferSubData(
		Buffers[vbo], 		// data
		colors_offset,		// offset
		sizeof(colors), 	// tamaño
		colors			// data
	);

	/**----------- Inicializar model_matrix_buffer --------*/

	// Se inicializan todas las instancias
	glNamedBufferStorage(
		Buffers[model_matrix_buffer], //buffer a usar
		INSTANCE_COUNT * sizeof(mat4), // tamaño total del buffer
		NULL, // se inicializa sin datos (se llena con datos más abajo)
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
		BUFFER_OFFSET(sizeof(positions))
	);

	glVertexAttribPointer(
		color_loc,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		BUFFER_OFFSET(colors_offset)
	);
    
	glEnableVertexAttribArray(position_loc);
	glEnableVertexAttribArray(normal_loc);
	glEnableVertexAttribArray(color_loc);
	// Permite que la asignación de colores sea por instancia
	// en lugar de por cada vértice.
	glVertexAttribDivisor(color_loc, 1);

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
		// asigna una matriz por instancia
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
}

/**
 * Se define una función update para actualizar las matrices
 * de modelo y de vista en cada frame.
*/
void update(float t) {

	// disminuye la velocidad de movimiento de los modelos
	t = t * 0.05; 

	mat4* matrices = (mat4*)glMapNamedBufferRange(
		Buffers[model_matrix_buffer],
		0,	
		INSTANCE_COUNT*sizeof(mat4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
	);
	
	// Las instancias giran con respecto al origen
	// y con respecto a su propio eje de manera desfazada
 	// entre si.
	for (int n = 0; n < INSTANCE_COUNT; n++) {
		float a = 50.0f * n / 4.0f;
		float b = 50.0f * n / 5.0f;
		float c = 50.0f * n / 6.0f;

		BEGIN_TRANSFORM(matrices[n])
			TRANSFORM(rotation_mat, a + t * 360.0f, 1.0f, 0.0f, 0.0f)
			TRANSFORM(rotation_mat, b + t * 360.0f, 0.0f, 1.0f, 0.0f)
			TRANSFORM(rotation_mat, c + t * 360.0f, 0.0f, 0.0f, 1.0f)
			TRANSFORM(translation_mat, 10.0f + a, 40.0f+b, 50.0f+c)
		END_TRANSFORM()
	}

	glUnmapNamedBuffer(Buffers[model_matrix_buffer]);
		
	mat4 view_matrix;
	
	// Rota la camara con respecto al origen
	BEGIN_TRANSFORM(view_matrix)
		TRANSFORM(translation_mat, 0.0f, 0.0f, -1500.0f)
		TRANSFORM(rotation_mat, t * 360.0f * 2.0f, 0.0f, 1.0f, 0.0f)
	END_TRANSFORM()
	
	glUniformMatrix4fv(render_view_matrix_loc, 1, GL_FALSE, view_matrix);
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
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, INSTANCE_COUNT);
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
