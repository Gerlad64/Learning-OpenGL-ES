
# Learning OpenGL Español
Repositorio donde compartiré mis avances mientras aprendo **OpenGL**, basándome en la novena edición del *Libro Rojo de OpenGL*.


Mi objetivo es desarrollar programas gráficos, como vieojuegos o simulaciones, utilizando OpenGL.
Además, quiero hacerlo principalmente en C y, posiblemente en Fortran, un lenguaje que me llama la 
atención. Por ahora, me siento más cómodo con estos lenguajes que con C++, que es el lenguaje utilizado en en el libro que estoy siguiendo.


El código usa variables en inglés, pero dejo comentarios en español para entender el propósito de cada línea de código.

## Sobre Esta Rama
Esta rama es para usuarios de Mac.

Esta versión funciona con OpenGL 4.1 en lugar de 4.5, por lo que habrán cambios con
respecto a la rama main. Por ejemplo, se usa la forma antigua para crear buffers:
```C
glGenVertexArrays(...);
glGenBuffers(...);
```
en vez de

```C
glCreateVertexArrays(...);
glCreateBuffers(...);
```

O para pasarle datos al Buffer:
```C 
glBufferData
```
en vez de
```C 
glNamedBufferStorage
```
donde la forma antigua requiere haber activado (bind) el buffer antes,
mientras que la nueva requiere pasarle el nombre.

## Dependencias
Se debe tener brew y clang instalados.
 Además, instalar glfw con el siguiente comando:
```bash
brew install glfw
```

## Compilación y Ejecución
El siguiente comando compila el objetivo en `build/Ch[num-capítulo]/[objetivo]`
```bash
make build [num-capítulo] [objetivo]
```
Para correr el programa compilado:
```bash
make run [num-capítulo] [objetivo]
```
Por ejemplo, `make build 1 triangles` compila `triangles.c` de `Chapters/1-Introduccion`,
el archivo compilado se encontrará en `build/Ch1/triangles`
