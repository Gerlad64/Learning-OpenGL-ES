#version 410 core

/*
    Se usa f como prefijo de "fragment"
*/
layout (location = 0) out vec4 fColor;


// uniform time recibido desde el programa
// no solo se limita al vertex program
uniform float time;

void main() {
    // pasa el tiempo a una unidad de ángulo
    // mejor efecto visual que pasar time
    float angle = time * 6.28318530718;
    // efecto visual que hace recorrer todo el ciclo RGB
    vec3 color = vec3(
	sin(angle) * 0.5 + 0.5,
        sin(angle + 2.094) * 0.5 + 0.5, // 120°
	sin(angle + 4.188) * 0.5 + 0.5  // 240°
    );

    fColor = vec4(color, 1.0);
}
