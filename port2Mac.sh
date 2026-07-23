#!/bin/bash


chapter=$1
target=$2

if [ -z ${chapter} ] && [ -z ${target} ]; then
	echo "usage: port2Mac [chapter-number] [program-name]"
fi

perl -0777 -pi -e '
	s/glCreateBuffers/glGenBuffers/g;
	s/glCreateVertexArrays/glGenVertexArrays/g;
	s/glCreateTextures/glGenTextures/g;

	s/GL_DYNAMIC_STORAGE_BIT/GL_DYNAMIC_DRAW/g;

	# Busca "glNamedBufferStorage(" y captura todo hasta la primera coma [^,]+
	# Se reemplaza por "glBufferData(SOME_FLAG"
	s/glNamedBufferStorage\s*\(\s*[^,]+/glBufferData(SOME_TARGET/gs;
	
	# Misma lógica que arriba
	s/glNamedBufferSubData\s*\(\s*[^,]+/glBufferSubData(SOME_TARGET/gs;
	s/glGetNamedBufferSubData\s*\(\s*[^,]+/glGetBufferSubData(SOME_TARGET/gs;
	s/glCopyNamedBufferSubData\s*\(\s*[^,]+,\s*[^,]+/glCopyBufferSubData(SOME_READ_TARGET, SOME_WRITE_TARGET/gs;

	# Agrega lineas necesarias luego de glfwInit
	s/(glfwInit\(\);)/$1\n\tglfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);\n\tglfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);/gs;
' Chapters/"${chapter}-"*/"${target}.c"

sed -i '' -e "s/450 core/410 core/g" "Chapters/${chapter}-"*/"${target}.vert"
sed -i '' -e "s/450 core/410 core/g" "Chapters/${chapter}-"*/"${target}.frag"
