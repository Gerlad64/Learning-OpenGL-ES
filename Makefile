# Configuraciones de compilación
CC = gcc
CFLAGS = -std=gnu23 -I./include
LIB_FLAGS = -lGL -ldl -lglfw


# tags del Makefile
KNOWN_COMMANDS := build run vim clean
# Parsear Argumentos
ARGS := $(filter-out $(KNOWN_COMMANDS),$(MAKECMDGOALS))
CHAPTER := $(word 1,$(ARGS))
TARGET := $(word 2,$(ARGS))

# convierte los argumentos parseados en reglas 
# (que no hacen nada) para evitar error de Make
ifneq ($(ARGS),)
  $(eval $(ARGS):;@:)
endif

# Error si no se proporcionan los argumentos CHAPTER Y TARGET
ifneq ($(filter build run vim,$(MAKECMDGOALS)),)
  # Verificamos si CHAPTER o TARGET están vacíos
  ifeq ($(CHAPTER),)
    $(error Error: Falta el capítulo. Uso: make <comando> <capitulo> <target>)
  endif
  ifeq ($(TARGET),)
    $(error Error: Falta target. Uso: make <comando> <capitulo> <target>)
  endif
endif

CHAPTER ?=
TARGET ?=

# Directorios
BUILD_DIR = build/Ch$(CHAPTER)
OBJ_DIR = build/ObjectFiles
BUILD_TARGET = $(BUILD_DIR)/$(TARGET)

# Archivos a compilar
LIB_SRC = lib/LoadShaders.c lib/gl3w.c
CH_SRC = $(wildcard Chapters/$(CHAPTER)-*/$(TARGET).c)
SRC = $(CH_SRC) $(LIB_SRC)

# Objetos
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

# vim
VIM_FILE := $(wildcard Chapters/$(CHAPTER)-*/$(TARGET))
NUM_FILES := $(words $(VIM_FILE))

# tag por defecto
all: help

# Compilación del ejecutable
build: $(BUILD_TARGET)

$(BUILD_TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $^ -o $@ $(LIB_FLAGS)

# Compilación de objetos
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Ejecutar
run: build
	$(BUILD_TARGET)

vim:
ifeq ($(NUM_FILES),0)
	@echo "No se encontró el archivo"
else ifneq ($(NUM_FILES),1)
	@echo "Varios archivos coinciden. Archivos encontrados:"; \
	echo "$(VIM_FILE)" | tr ' ' '\n'
else
	@vim "$(VIM_FILE)"
endif


# Limpiar todo
clean:
	rm -rf build

help:
	@echo hola 

