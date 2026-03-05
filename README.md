
# Learning OpenGL Español
Repositorio donde compartiré mis avances mientras aprendo **OpenGL**, basándome en la novena edición del *Libro Rojo de OpenGL*.


Mi objetivo es desarrollar programas gráficos, como vieojuegos o simulaciones, utilizando OpenGL.
Además, quiero hacerlo principalmente en C y, posiblemente en Fortran, un lenguaje que me llama la 
atención. Por ahora, me siento más cómodo con estos lenguajes que con C++, que es el lenguaje utilizado en en el libro que estoy siguiendo.


El código usa variables en inglés, pero dejo comentarios en español para entender el propósito de cada línea de código.



## Dependencias
```bash
sudo apt install build-essential libglfw3-dev
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
