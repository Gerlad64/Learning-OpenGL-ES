/**************************************************************
 PROGRAMA QUE DIBUJA 2 TRIÁNGULOS EN PANTALLA

	FLAGS DE COMPILACIÓN NECESARIAS: 
	-lGL -ldl -lglfw

***************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "LoadShaders.h"

#define BUFFER_OFFSET(offset)((void *)(offset))

/*
	Crea índices con enum para acceder a objetos de VAOs y Buffers.
	por ejemplo, si VAO_IDs fuera { Triangles, Squares, ..., NumVAOs},
	es equivalente a que Triangles sea 0, Squares sea 1, y así sucesivamente
	hasta llegar a NumVAOs, que siempre será igual a la cantidad de objetos.
*/
enum VAO_IDs { Triangles, NumVAOs }; // Triangles == 0, NumVAOs == 1
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

constexpr GLuint NumVertices = 6;

void init() {
    static const GLfloat vertices[NumVertices][2] = 
    {
        // Triangulo 1
        { -0.90, -0.90 }, 
        {  0.85, -0.90 }, 
        { -0.90,  0.85 }, 
        // Triangulo 2
        {  0.90, -0.85 }, 
        {  0.90,  0.90 }, 
        { -0.85,  0.90 }, 
    };

    /* void glCreateVertexArrays(GLsizei n, GLuint *arrays)
       void glCreateBuffers(GLsizei n, GLuint *buffers)
        Llena los arrays VAOs y Buffers con identificadores numericos "names"
        para los objetos que guarda. O sea, el array VAOs guarda identificadores
        de "Vertex Arrays" y Buffers guarda identificadores de Buffers.
    */
    glCreateVertexArrays(NumVAOs, VAOs); 
    glCreateBuffers(NumBuffers, Buffers);

    /*
        Reserva memoria en GPU  para guardar datos asociados
        al buffer indicado, y copia los datos de 'vertices'.

        El último parámetro de tipo GLbitfield
        provee de manera adicional información sobre
        cómo serán usados los datos guardados.
    */ 
    glNamedBufferStorage(
        Buffers[ArrayBuffer],   // "name" donde se van a guardar los datos
		sizeof(vertices),       // tamaño "size" en bytes
		vertices,               // datos "const void *data"
	    0                       // No se selecciona ninguna opción
    );                          // Podría ser GL_FOO | GL_BAR | ...
    /*
        Cargar Vertex y Fragment shaders usando la estructura
        ShaderInfo y función LoadShaders de LoadShaders.h
    */
    ShaderInfo shaders[] = 
    {
        { GL_VERTEX_SHADER,   "Chapters/1-Introduccion/triangles.vert" },
        { GL_FRAGMENT_SHADER, "Chapters/1-Introduccion/triangles.frag" },
        { GL_NONE, NULL },
    };
    GLuint program = LoadShaders(shaders); 
    glUseProgram(program);

    /*
        """Activa""" algun objeto en particular por el id pasado.
    */
    glBindVertexArray(VAOs[Triangles]);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);

    /* 
        Pasarle datos al vertex program
    */
    /*
	Configura cómo ls GPU debe leer los datos del buffer guardados en memoria 
	para el atributo 'vPosition'. Funcionará siempre que los datos esten organizados 
	de manera contigua o intercalada (no así como las listas enlazadas).

	Esta configuración **se guarda en el VAO activo** y será usada por la GPU durante
    	los draw calls (glDrawArrays, glDrawElements), no involucra a la CPU directamente.	
    */
    glVertexAttribPointer(
        vPosition,          // index: el valor de "location" dentro del shader, o sea, 
                            // layout (location = 0) en el archivo triangles.vert
		2,                  // size: cantidad de elementos en un vertice, en este caso 2: x e y
		GL_FLOAT,           // type: ...
		GL_FALSE,           // normalized: ... 
		0,                  // stride(similar a step): En este caso, los datos no están separados
                            // por lo que no se necesita realizar ningún salto. 
		BUFFER_OFFSET(0)    // pointer: Los datos comienzan desde 0 en lugar de otro número.
                            // BUFFER_OFFSET representa la siguiente macro 
                            //      #define BUFFER_OFFSET(offset)((void *)(offset))
    );
    /*
        Activa el vertex-attribute array
        si se quisiera desactivar se usaría 
            glDisableVertexAttribArray
    */
    glEnableVertexAttribArray(vPosition);

}

void display() {
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray(VAOs[Triangles]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}


int main() {
    glfwInit(); // Inicializa glfw

    GLFWwindow* window = glfwCreateWindow(640, 480, "My First Triangles", NULL, NULL); // configura una ventana
    glfwMakeContextCurrent(window); // establece que "window" es el contexto actual, o sea, que todos los comandos de OpenGL son hacia esta ventana   
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
