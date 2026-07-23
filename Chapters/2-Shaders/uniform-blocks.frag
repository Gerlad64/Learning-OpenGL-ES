#version 450 core

layout (location = 0) out vec4 fColor;

layout(std140) uniform Uniforms {
	float x;
	float y;
	float z;
	float scale;
	float rotation;
	int enabled;
} my_uniform;

void main() {
        fColor = vec4(1.0, 0.0, 0.0, 1.0);
}
