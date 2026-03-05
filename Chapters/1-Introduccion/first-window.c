/**************************************************************
    PROGRAMA QUE CREA UNA VENTANA DE GLFW
	FLAGS DE COMPILACIÓN NECESARIAS: 
		-lglfw
***************************************************************/
#include <GLFW/glfw3.h>

int main() {
    glfwInit(); // Inicializa glfw

    GLFWwindow* window = glfwCreateWindow(640, 480, "My First Window", NULL, NULL); // configura una ventana
    glfwMakeContextCurrent(window); // establece que "window" es el contexto actual, o sea, que todos los comandos de OpenGL son hacia esta ventana   

//    init();

    while ( !glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window); //redibuja los contenidos en pantalla
        glfwPollEvents(); //revisa mensajes del SO
    }
    glfwDestroyWindow(window);
    
    glfwTerminate();

    return 0;
}
