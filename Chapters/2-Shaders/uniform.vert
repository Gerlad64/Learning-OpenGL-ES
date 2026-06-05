#version 450 core 

#define TAU 6.283185307179586

uniform float time;

/* igual que en triangles.vert del capítulo 1*/ 
layout (location = 0) in vec4 vPosition;

void main() {
	gl_PointSize= 50.0;
	gl_Position = vec4(vPosition.x, sin(time * TAU), vPosition.zw);
}
