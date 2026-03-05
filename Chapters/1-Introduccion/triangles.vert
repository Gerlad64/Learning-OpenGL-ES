/**
    Vertex Program triangles.vert del programa triangles.c
*/
// Utiliza GLSL asociado a OpenGL 4.5
#version 450 core 

/*
    Definimos una variable de shader "shader variable".
    Sirve como un puente entre la CPU y GPU. En este caso,
    tenemos una variable de tipo input "in" la cual será populada
    por la CPU mediante buffers y leida por la GPU.

    vPosition:
    Como convensión, se usa el prefijo "v" para indicar que la variable
    guarda información posicional de un vértice (vertex).
    
    vec4:
    vector de 4 dimensiones de tipo float.
    En triangles.c se especifican vertices con solo 2 coordenadas,
    pues OpenGl rellena automáticamente según el valor por defecto
    de vec4 que es (0.0, 0.0, 0.0, 1.0)

    layout:
    Esta parte se conoce como "layout qualifier", provee información adicional 
    (meta-data) a la declaración de la variable. Existen varias opciones para 
    establecer. En este caso, se deja el atributo location en 0.
    
*/ 
layout (location = 0) in vec4 vPosition;

void main() {
    gl_Position = vPosition;
}
