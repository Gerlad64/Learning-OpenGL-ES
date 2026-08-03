#version 410 core 
 
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec3 vColor;

out vec4 color;

void main() {
    color = vec4(vColor.xyz, 1.0);
    gl_Position = vPosition;
}
