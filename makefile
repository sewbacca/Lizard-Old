
.PHONY: all exec tests

ifndef VERBOSE
.SILENT:
endif

ifndef CONFIG
CONFIG := release
endif

# Linux

MAKE := /usr/bin/make
CXX := /usr/bin/g++
SHELL := /bin/bash

# Directories

PROJECT_NAME := Lizard
DIR_SRC := src
DIR_TESTS := tests
DIR_OBJ = bin/obj/$(CONFIG)
DIR_BIN = bin/$(CONFIG)
FILE_MAIN := src/main.cpp

DEP_SRC := -lpthread

# Flags

FLAGS := --std=c++17 -Wall -Wextra -Iinclude -Isrc
ifeq ($(CONFIG), release)
FLAGS += -O3
endif

ifeq ($(CONFIG), debug)
FLAGS += -g
endif

# Resolve files
# FILES_SRC contains all file names without .cpp and without $(FILE_MAIN).cpp

FILES_SRC = $(filter-out $(FILE_MAIN), $(wildcard $(DIR_SRC)/*.cpp))
FILES_TESTS = $(wildcard $(DIR_TESTS)/*.cpp)

all: exec tests

# Builds executable
exec: $(patsubst %.cpp,$(DIR_OBJ)/%.o,$(FILE_MAIN) $(FILES_SRC))
	mkdir -p $(DIR_BIN)
	echo Linking $(DIR_BIN)/$(PROJECT_NAME)
	$(CXX) -o $(DIR_BIN)/$(PROJECT_NAME) $^ $(DEP_SRC)
	echo Successfully built executable

# Builds executable
tests: $(patsubst %.cpp,$(DIR_OBJ)/%.o,$(FILES_TESTS) $(FILES_SRC))
	mkdir -p $(DIR_BIN)
	echo Linking $(DIR_BIN)/tests
	$(CXX) -o $(DIR_BIN)/tests $^ $(DEP_SRC)
	echo Successfully built tests

$(DIR_OBJ)/%.o: %.cpp
	mkdir -p $(dir $@)
	echo Compiling $<...
	$(CXX) $(FLAGS) -c -o $@ $<

clean:
	rm bin -rv