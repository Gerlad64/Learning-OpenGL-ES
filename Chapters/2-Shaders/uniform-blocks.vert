#version 410 core 


layout(std140) uniform Uniforms {
	float x;
	float y;
	float z;
	float scale;
	float rotation;
	int enabled;
} my_uniform;


/* igual que en triangles.vert del capítulo 1*/ 
layout (location = 0) in vec4 vPosition;

void main() {
	gl_PointSize= my_uniform.scale;
	gl_Position = vec4(my_uniform.x, my_uniform.y, my_uniform.z, 1);
}
