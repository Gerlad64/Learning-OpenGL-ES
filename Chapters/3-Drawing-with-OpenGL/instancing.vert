#version 410 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in mat4 model_matrix;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;


out VERTEX {
	vec3  normal;
	vec4  color;

} vertex;


void main() {
	mat4 model_view_matrix = view_matrix * model_matrix;
	gl_Position = projection_matrix * (model_view_matrix * vec4(position, 1.0));
 	// representa donde apunta la normal luego de las transformaciones
	// y desde la vista de la camara
	// como la matriz de modelo y los colores son por cada instancia,
	// entonces, se tienen multiples normales y colores que pasan al
	// fragment shader.
	vertex.normal = mat3(model_view_matrix) * normal;
	vertex.color = vec4(color, 1.0);
}
