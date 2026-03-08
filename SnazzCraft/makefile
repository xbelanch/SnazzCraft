CPPC = g++
CC = gcc
INCLUDES = includes
CPPFLAGS = -I$(INCLUDES) -Werror
CFLAGS = -I$(INCLUDES)
LDFLAGS = -lglfw -ldl -lGL -lnoise -lm -std=c++23

# 1. Automatically find all .cpp and .c files recursively
# This finds every file in src/ and includes/ without you listing them
CPP_SOURCES := $(shell find src -name '*.cpp')
C_SOURCES   := $(shell find includes -name 'glad.c')

# 2. Map source files to the 'build' directory while PRESERVING folder structure
# src/engine/core/core.cpp -> build/src/engine/core/core.o
OBJS := $(CPP_SOURCES:%.cpp=build/%.o) $(C_SOURCES:%.c=build/%.o)

all: bin/executable

# Linking: $^ represents all the object files listed in $(OBJS)
bin/executable: $(OBJS)
	$(CPPC) $^ -o $@ $(LDFLAGS)

# Rule for C++: Creates the subfolder in 'build' before compiling
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPPC) $(CPPFLAGS) -c $< -o $@

# Rule for C: Same logic for .c files
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build bin/executable

.PHONY: all clean
