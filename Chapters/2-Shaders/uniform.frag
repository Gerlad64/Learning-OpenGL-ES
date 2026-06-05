#version 410 core

/*
    Se usa f como prefijo de "fragment"
*/
layout (location = 0) out vec4 fColor;

void main() {
    fColor = vec4(0.5, 0.4, 0.8, 1.0);
}
