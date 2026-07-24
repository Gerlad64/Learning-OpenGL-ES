#version 450 core 

uniform mat4 model_matrix;
uniform mat4 projection_matrix;
 
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vColor;

out vec4 color;

void main() {
    color = vec4(vColor.xyz, 1.0);
    gl_Position = projection_matrix * model_matrix * vPosition;
}
