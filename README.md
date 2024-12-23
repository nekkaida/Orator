# Orator

**orator** is an OpenGL/GLUT application that renders a 3D speaker-like shape (a spherical cap with a flat base and concave center) **and** plays audio using **SDL2**. It showcases:

- **3D Rendering** with OpenGL/GLUT  
- **Planar Shadow** projection  
- **Audio Playback** via SDL2  

## Features

- **Spherical Cap** with a **flat ring** and **concave center**  
- **Planar Shadow** cast on a “floor”  
- **Checkerboard Texture** (toggle-able)  
- **Simple Keyboard & Mouse** controls for toggling features and rotating the speaker  
- **Sound Playback**: Uses SDL2 to load and play a `.wav` file

---

## Requirements

1. **OpenGL** + **GLUT**  
   - Ensure your system has OpenGL drivers installed.  
   - For GLUT, you can use **freeglut** or the original GLUT library.

2. **SDL2** (for Audio)  
   - To install:  
     - **Windows + vcpkg**: `vcpkg install sdl2`  
     - **Linux (Debian/Ubuntu)**: `sudo apt-get install libsdl2-dev`  
     - **macOS (Homebrew)**: `brew install sdl2`  

3. A **C++ compiler** that can link against OpenGL, GLUT, and SDL2 libraries.

---

## Building orator

1. **Clone or download** this repository.  
2. **Install** all required dependencies (OpenGL/GLUT, SDL2) as listed above.  
3. **Compile** the code. On many systems, a command-line example might look like:

   ```bash
   g++ orator.cpp -lGL -lGLU -lglut -lSDL2 -o orator
   ```
   Where:
   - `orator.cpp` is your main source code.  
   - `-lSDL2` links the SDL2 library.  
   - Adjust the include and library paths if needed (e.g., `-I/path/to/headers -L/path/to/libs`).

4. **Run** the resulting `orator` (or `orator.exe` on Windows) executable:

   ```bash
   ./orator
   ```

---

## Running orator

When you launch **orator**, a window will open with:

- A **3D speaker** model,  
- A **floor** for shadow projection,  
- **Sound** playing in the background, assuming you have a valid `.wav` file specified in the code.

### Command-line Arguments

If your code is set up to accept a `.wav` file as an argument (optional pattern), you could do:

```bash
./orator mySound.wav
```

*(Otherwise, you can hardcode the WAV filename in the source code.)*

### Keyboard Controls (example)

- **ESC** – Quit the application.  
- **t** – Toggle texture mapping.  
- **s** – Toggle smooth/flat shading.  
- **d** – Toggle depth testing.  
- **(Arrow Keys)** – Adjust camera angles if implemented via special keys.  

### Mouse Controls (example)

- **Left-Click + Drag** – Rotate the speaker model in real time.  

*(These are just examples; confirm with your source code’s actual key/mouse bindings.)*

---

## Audio Notes

- **SDL2** is initialized at startup.  
- A **WAV file** is loaded and queued for playback.  
- For more advanced audio (e.g., looping, MP3/OGG), consider using [**SDL2_mixer**](https://www.libsdl.org/projects/SDL_mixer/).

---

## Troubleshooting

- **SDL2 not found**  
  - Verify your system path/linker flags and that SDL2 is correctly installed.  
- **No sound**  
  - Check that your `.wav` file exists and the path in code is correct.  
  - Confirm that your audio device is functional.  
- **Blank Window** or **GL errors**  
  - Make sure OpenGL and GLUT are installed.  
  - Update or fix your graphics drivers.

---

## Contributing

- Feel free to submit pull requests or issues if you find bugs or want new features.

---

**Enjoy orator!**  