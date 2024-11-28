# Orator

![Spherical Cap](https://via.placeholder.com/800x400?text=Spherical+Cap+Renderer+Screenshot)

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
  - [1. Clone the Repository](#1-clone-the-repository)
  - [2. Install Dependencies](#2-install-dependencies)
  - [3. Build the Project](#3-build-the-project)
- [Usage](#usage)
  - [Running the Application](#running-the-application)
- [Controls](#controls)
- [Code Structure](#code-structure)
- [Customization](#customization)
- [Troubleshooting](#troubleshooting)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Introduction

The **Orator** is a C/C++ application that leverages OpenGL and GLUT to render a dynamic 3D model composed of a spherical cap with a flat base and a concave center. This project serves as an educational tool to demonstrate fundamental OpenGL concepts, including texture mapping, lighting, shading, and interactive user controls for manipulating the 3D object.

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

#### Linux (Debian-based distributions)

Open a terminal and execute the following commands:

```bash
sudo apt-get update
sudo apt-get install build-essential freeglut3-dev
