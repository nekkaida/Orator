
---

## Makefile

```makefile
# Makefile for OpenGL Spherical Cap Renderer

# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -Wextra -O2

# Source Files
SRCS = final-project.cpp

# Object Files
OBJS = $(SRCS:.cpp=.o)

# Executable Name
TARGET = final-project

# Libraries
LIBS = -lGL -lGLU -lglut -lm

# Default Target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony Targets
.PHONY: all clean
