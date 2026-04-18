# 3D Renderer

A software 3D renderer written from scratch in C using SDL2. No GPU acceleration — everything is drawn pixel-by-pixel into a color buffer.

## What it does

Renders a rotating 9x9x9 point cloud cube using perspective projection. Points are transformed in 3D (rotation around all three axes), projected onto 2D, and drawn to the screen at 30 FPS.

## Building

Requires CMake and SDL2.

```sh
make        # configures and builds via cmake
make run    # runs the renderer
make clean  # removes the build directory
```

## Project structure

```
src/
  main.c      - game loop, setup, projection, and rendering
  display.c/h - window management, color buffer, and drawing primitives
  vector.c/h  - vec2/vec3 types and rotation functions
```
