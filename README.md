# Orator
## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
  - [Hardware](#hardware)
  - [Software](#software)
- [Installation](#installation)
  - [1. Clone the Repository](#1-clone-the-repository)
  - [2. Install Dependencies](#2-install-dependencies)
  - [3. Build the Project](#3-build-the-project)
    - [Option 1: Using Makefile](#option-1-using-makefile)
    - [Option 2: Manual Compilation](#option-2-manual-compilation)
  - [4. Clean Build Files](#4-clean-build-files)

## Introduction

The **OpenGL Spherical Cap Renderer** is a C++ application that leverages OpenGL and GLUT to render a dynamic 3D model composed of a spherical cap with a flat base and a concave center. This project serves as an educational tool to demonstrate fundamental OpenGL concepts, including texture mapping, lighting, shading, and interactive user controls for manipulating the 3D object.

## Features

- **Spherical Cap Rendering**: Displays a smooth spherical cap with adjustable parameters for size and depth.
- **Texture Mapping**: Applies a generated checkerboard texture to enhance visual appearance.
- **Lighting and Shading**: Implements basic lighting with options for smooth (`GL_SMOOTH`) and flat (`GL_FLAT`) shading.
- **Interactive Controls**:
  - **Mouse Rotation**: Click and drag to rotate the model along the X and Y axes.
  - **Keyboard Toggles**: Enable/disable textures, smooth shading, and depth testing.
- **Automatic Rotation**: The model rotates automatically when not being interacted with.
- **Concave Center**: Features a concave indentation at the center of the flat base for added complexity.

## Requirements

### Hardware

- **Graphics Card**: Any modern GPU that supports OpenGL.
- **Display**: Monitor capable of displaying the application's window.

### Software

- **Operating System**: Linux, macOS, or Windows (with appropriate OpenGL and GLUT support).
- **Compiler**: `g++` (GNU C++ Compiler) supporting C++11 or later.
- **Libraries**:
  - **OpenGL**: For graphics rendering.
  - **GLUT**: OpenGL Utility Toolkit for windowing and input handling.
  - **Math Library**: For mathematical operations.

### Installing Dependencies

#### Linux (Debian-based Distributions)

Open a terminal and execute the following commands:

```bash
sudo apt-get update
sudo apt-get install build-essential freeglut3-dev
```

#### macOS

Use [Homebrew](https://brew.sh/) to install GLUT:

```bash
brew update
brew install freeglut
```

#### Windows

For Windows, you can use [MSYS2](https://www.msys2.org/) to install the necessary libraries:

1. **Install MSYS2**: Download and install from the [MSYS2 Official Website](https://www.msys2.org/).
2. **Update Package Database and Base Packages**:

   Open the MSYS2 terminal and run:

   ```bash
   pacman -Syu
   ```

3. **Install Development Tools and Libraries**:

   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freeglut
   ```

4. **Add MSYS2 to System Path**: Ensure that the `mingw64` binaries are in your system's `PATH` environment variable.

## Installation

Follow these steps to set up and build the **OpenGL Spherical Cap Renderer** on your system.

### 1. Clone the Repository

Open your terminal (or Git Bash on Windows) and execute:

```bash
git clone https://github.com/yourusername/spherical-cap-renderer.git
cd spherical-cap-renderer
```

*Replace `yourusername` with your actual GitHub username if applicable.*

### 2. Install Dependencies

Ensure that all required libraries are installed as per the [Requirements](#requirements) section.

### 3. Build the Project

You have two options to build the project: using the provided **Makefile** or compiling manually with `g++`.

#### Option 1: Using Makefile

The provided **Makefile** automates the build process. To compile the project, run:

```bash
make
```

This command will generate an executable named `final-project`.

#### Option 2: Manual Compilation

Alternatively, you can compile the project manually using the `g++` command:

```bash
g++ -o final-project final-project.cpp -lGL -lGLU -lglut -lm
```

**Explanation of Flags and Options**:

- `g++`: Invokes the GNU C++ Compiler.
- `-o final-project`: Specifies the output executable name.
- `final-project.cpp`: Source file to compile.
- `-lGL`: Links the OpenGL library.
- `-lGLU`: Links the OpenGL Utility Library.
- `-lglut`: Links the GLUT library.
- `-lm`: Links the math library.

*Ensure that the source file name matches your actual source code file (`final-project.cpp`).*

### 4. Clean Build Files

To remove compiled object files and the executable, use:

```bash
make clean
```

This command deletes `final-project.o` and `final-project`, ensuring a clean state for future builds.

## Usage

After successfully building the project, you can run the application to visualize the 3D spherical cap.

### Running the Application

Navigate to the project directory in your terminal and execute:

```bash
./final-project
```

This command launches the application, opening a window titled **"OpenGL Spherical Cap with Flat Base and Concave Center"**, displaying the rendered 3D model.

## Controls

Interact with the 3D model using mouse and keyboard inputs to explore its features.

### Mouse Controls

- **Rotate**: Click and drag the **left mouse button** to rotate the model along the X and Y axes.

### Keyboard Controls

- **`t`**: Toggle **Texture Mapping** on/off.
- **`s`**: Toggle between **Smooth Shading** (`GL_SMOOTH`) and **Flat Shading** (`GL_FLAT`).
- **`d`**: Toggle **Depth Testing** on/off.
- **`Esc`**: **Exit** the application.

*Note*: The application supports real-time interaction. Changes take effect immediately upon key presses.

## Code Structure

The project is organized into the following files and directories:

```
spherical-cap-renderer/
├── Makefile
├── README.md
├── final-project.cpp
└── LICENSE
```

### File Descriptions

- **final-project.cpp**: Contains the entire source code for rendering the 3D model, handling user interactions, and managing OpenGL states.
- **Makefile**: Automates the build process, compiling the source code and linking necessary libraries.
- **README.md**: Provides an overview, setup instructions, and usage guidelines for the project.
- **LICENSE**: Specifies the licensing terms for the project.

*Ensure that all files are placed in the root directory of the project.*

## Customization

Customize various aspects of the 3D model and its rendering by modifying the source code.

### Shape Parameters

In `final-project.cpp`, you can adjust the following constants to alter the model's appearance:

```cpp
const float phi_max = (3.0f * M_PI) / 4.0f; // Adjust for desired cap size
const float innerRadiusFactor = 0.4f;        // Factor to determine inner ring size
const float concaveDepth = 0.1f;             // Depth of the concave indentation
```

- **`phi_max`**: Controls the angular extent of the spherical cap. Increasing this value makes the cap larger.
- **`innerRadiusFactor`**: Determines the size of the inner ring relative to the outer radius. Lower values create a smaller inner ring.
- **`concaveDepth`**: Sets the depth of the concave indentation at the center. Higher values result in a deeper concave area.

### Texture Modification

Modify the `generateTexture()` function to create different textures:

```cpp
void generateTexture() {
    // Existing checkerboard texture generation code
    // Modify color patterns or switch to different algorithms for unique textures
}
```

*Example*: Change the checkerboard to a striped pattern by altering the texture generation logic.

### Lighting and Material Properties

Adjust lighting and material settings in the `init()` and `display()` functions to change the model's appearance under different lighting conditions.

```cpp
// In init()
GLfloat light_pos[] = { 5.0, 5.0, 5.0, 0.0 };
glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

// In display()
GLfloat mat_ambient[]   = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_diffuse[]   = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_specular[]  = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
```

*Experiment*: Change `light_pos` to reposition the light source or adjust material properties for different reflective effects.

## Troubleshooting

Encountering issues during installation or execution? Below are common problems and their solutions.

### Makefile Errors

**Error Message:**

```
Makefile:2: *** missing separator.  Stop.
```

**Cause:**

- **Incorrect Indentation**: Makefiles require **tabs**, not spaces, to indent command lines under targets.

**Solution:**

1. **Open the Makefile** in a text editor that clearly distinguishes tabs and spaces (e.g., VS Code, Sublime Text).

2. **Ensure Proper Indentation**:
   - Command lines (e.g., compilation commands) must start with a **tab**.
   - Replace spaces with tabs where necessary.

**Correct Makefile Example:**

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
```

*Note*: The lines under `$(TARGET): $(OBJS)` and `%.o: %.cpp` must start with a **tab**, not spaces.

### Compilation Errors

**Common Issues:**

- **Missing Libraries**: Ensure all required libraries (`GL`, `GLU`, `glut`, `math`) are installed.
- **Incorrect Source File Name**: Verify that the source file name in the Makefile matches your actual file.
- **Syntax Errors**: Check your source code for any syntax errors that the compiler reports.

**Solution:**

- Revisit the [Installation](#installation) section to ensure all dependencies are correctly installed.
- Double-check the Makefile for correct source file naming and paths.
- Review compiler error messages and address the highlighted issues in your code.

### Runtime Issues

**Issues:**

- **No Window Appears**: The application may exit immediately or fail to create a window.
- **Visual Artifacts**: Strange colors, flickering, or incomplete rendering.

**Solutions:**

- **No Window Appears**:
  - Ensure your system supports OpenGL.
  - Verify that GLUT is correctly installed.
  - Check for any runtime errors in the terminal output.

- **Visual Artifacts**:
  - Ensure depth testing and shading modes are correctly toggled.
  - Verify that normals are accurately calculated in the rendering functions.

## License

This project is licensed under the [MIT License](LICENSE). You are free to use, modify, and distribute this software as per the license terms.

## Acknowledgments

- **OpenGL**: For providing a powerful graphics rendering API.
- **GLUT**: For simplifying windowing and input handling in OpenGL applications.
- **The OpenGL Community**: For extensive tutorials and resources that facilitated the development of this project.
- **GitHub**: For hosting and version-controlling this project, enabling collaboration and contribution.

---

Feel free to contribute to this project by submitting issues or pull requests. For major changes, please open an issue first to discuss what you would like to change.

---

## Makefile

Ensure that your `Makefile` is correctly indented using **tabs**, not spaces. Here's the complete `Makefile` for your project:

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
```

**Important Notes:**

1. **Tab Indentation**: 
   - Commands under targets (`$(TARGET): $(OBJS)` and `%.o: %.cpp`) must start with a **tab** character.
   - Do **not** use spaces for indentation in Makefiles.

2. **File Naming**:
   - Ensure the file is named exactly `Makefile` or `makefile` (case-sensitive in Unix-like systems).

3. **Line Endings**:
   - Use Unix-style line endings (`LF`). Convert Windows-style (`CRLF`) if necessary using `dos2unix`:
     ```bash
     dos2unix Makefile
     ```

4. **Saving the Makefile**:
   - Save the Makefile in the root directory of your project (`spherical-cap-renderer/`).

---

# Example: Building and Running the Project

Here's a step-by-step example of building and running your project using both the Makefile and manual compilation.

### Using the Makefile

1. **Navigate to Project Directory**:
   ```bash
   cd ~/Codes/computer-graphics-lab/final-project
   ```

2. **Build the Project**:
   ```bash
   make
   ```
   - **Expected Output**: The Makefile will compile `final-project.cpp` and generate the executable `final-project`.
   - **If Successful**: No errors will be displayed, and you should see the `final-project` executable in your directory.

3. **Run the Application**:
   ```bash
   ./final-project
   ```
   - A window titled **"OpenGL Spherical Cap with Flat Base and Concave Center"** should appear, displaying the rendered 3D model.

4. **Clean Build Files** (Optional):
   ```bash
   make clean
   ```
   - This command removes the compiled object files and the executable, allowing for a fresh build.

### Manual Compilation

If you prefer not to use the Makefile, you can compile the project manually:

1. **Navigate to Project Directory**:
   ```bash
   cd ~/Codes/computer-graphics-lab/final-project
   ```

2. **Compile the Project**:
   ```bash
   g++ -o final-project final-project.cpp -lGL -lGLU -lglut -lm
   ```

3. **Run the Application**:
   ```bash
   ./final-project
   ```

4. **Clean Build Files** (Manual Step):
   ```bash
   rm final-project
   ```
   - Since this method doesn't generate object files, you only need to remove the executable.

