
all: notification tests exec

ifndef VERBOSE
.SILENT:
endif

# Compiler Args
CXX = g++
CXXFLAGS = --std=c++17 -Wall -c -Winline -Wextra -pedantic

# Naming

PROJECT_NAME = Lizard
PROG_EXT =

# Folder

OBJ_DIR = bin/obj
BIN_DIR = bin

SRC = src
SRC_MAIN = $(SRC)/main.cpp
OBJ_MAIN = $(OBJ_DIR)/$(SRC)/main.o
TESTS = tests

# Includes
INCLUDE_GLOBAL = -Isrc -Iinclude
INCLUDE_TEST = -Itest

# LD Flags
LDFLAGS_GLOBAL = -lpthread
LDFLAGS_TEST =

# Definitions

ifdef MINGW
	OBJ_DIR := $(OBJ_DIR)/mingw
	BIN_DIR := $(BIN_DIR)/mingw
endif

ifdef DEBUG
	CXXFLAGS := $(CXXFLAGS) -g
	OBJ_DIR := $(OBJ_DIR)/debug
	BIN_DIR := $(BIN_DIR)/debug
endif

ifndef DEBUG
	CXXFLAGS := $(CXXFLAGS) -DNDEBUG
endif

ifdef RELEASE
	CXXFLAGS := $(CXXFLAGS) -O3
	OBJ_DIR := $(OBJ_DIR)/release
	BIN_DIR := $(BIN_DIR)/release
endif

# Generated

CPP_SRC = $(shell find $(SRC) -type f -name \*.cpp | grep -v $(SRC_MAIN))
CPP_TESTS = $(shell find $(TESTS) -type f -name \*.cpp)

OBJ_SRC = $(subst $(SRC),$(OBJ_DIR)/$(SRC),$(CPP_SRC:.cpp=.o))
OBJ_TESTS = $(subst $(TESTS),$(OBJ_DIR)/$(TESTS),$(CPP_TESTS:.cpp=.o))

DEP_SRC = $(OBJ_SRC:.o=.d)
DEP_MAIN = $(OBJ_MAIN:.o=.d)
DEP_TESTS = $(OBJ_TESTS:.o=.d)

# Targets

-include $(DEP_SRC)
-include $(DEP_MAIN)
-include $(DEP_TESTS)

notification:
ifdef MINGW
	echo MinGW enabled
endif
ifdef WINDOWS
	echo Compiling for Windows
endif
ifdef LINUX
	echo Compiling for LINUX
endif
ifdef DEBUG
	echo Debugging build
endif
ifdef RELEASE
	echo Release build
endif

tests: notification $(BIN_DIR)/test$(PROG_EXT)
exec: notification $(BIN_DIR)/$(PROJECT_NAME)$(PROG_EXT)

clear-bin:
	rm -r $(BIN_DIR)/*

clear-obj:
	rm -r $(OBJ_DIR)

clear-dep:
	rm -f $(DEP_SRC) $(DEP_MAIN) $(DEP_TESTS)

clear: clear-bin clear-obj

# Linking

$(BIN_DIR)/$(PROJECT_NAME)$(PROG_EXT): $(OBJ_SRC) $(OBJ_MAIN)
	mkdir -p $(@D)
	echo Linking $@
	$(CXX) -o $@ $^ $(LDFLAGS_GLOBAL)

$(BIN_DIR)/test$(PROG_EXT): $(OBJ_TESTS) $(OBJ_SRC)
	mkdir -p $(@D)
	echo Linking $@
	$(CXX) -o $@ $^ $(LDFLAGS_GLOBAL) $(LDFLAGS_TEST)

# Dependencies

$(OBJ_DIR)/$(SRC)/%.d: $(SRC)/%.cpp
	echo Reading deps for $<
	mkdir -p $(@D)
	$(CXX) $(INCLUDE_GLOBAL) -MM -MT "$@ $(patsubst %.d,%.o,$@)" -MF $@ $<

$(OBJ_DIR)/$(TESTS)/%.d: $(TESTS)/%.cpp
	echo Reading deps for $<
	mkdir -p $(@D)
	$(CXX) $(INCLUDE_GLOBAL) $(INCLUDE_TEST) -MM -MT "$@ $(patsubst %.d,%.o,$@)" -MF $@ $<

# Compiling

$(OBJ_DIR)/$(SRC)/%.o: $(SRC)/%.cpp
	mkdir -p $(@D)
	echo Compiling $<
	$(CXX) $(CXXFLAGS) $(INCLUDE_GLOBAL) -o $@ $<

$(OBJ_DIR)/$(TESTS)/%.o: $(TESTS)/%.cpp
	mkdir -p $(@D)
	echo Compiling $<
	$(CXX) $(CXXFLAGS) $(INCLUDE_GLOBAL) $(INCLUDE_TEST) -o $@ $<
