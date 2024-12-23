#########################################
# Makefile for the "orator" Project
# 
# - Builds an executable called "orator"
# - Links against OpenGL, GLUT, and SDL2
#########################################

# Compiler
CXX       = g++

# Compiler flags:
#   -Wall      : enable common compiler warnings
#   -std=c++11 : enable C++11 features (adjust as needed)
CXXFLAGS  = -Wall -std=c++11

# Libraries to link against:
#   -lGL   : OpenGL
#   -lGLU  : OpenGL Utility
#   -lglut : GLUT
#   -lSDL2 : SDL2
LIBS      = -lGL -lGLU -lglut -lSDL2

# If SDL2 or other libraries are in non-standard paths, add them here:
#   Example:
#   CXXFLAGS += -I/usr/local/include
#   LIBS     += -L/usr/local/lib

# Name of the final executable
TARGET    = orator

# Source files (add more .cpp files if you have them)
SOURCES   = orator.cpp

#########################################
# Default rule
#########################################
all: $(TARGET)

#########################################
# Build the executable
#########################################
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

#########################################
# Clean rule - remove the executable
#########################################
clean:
	rm -f $(TARGET)
