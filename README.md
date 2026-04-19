# 3D Renderer

A software 3D renderer written from scratch in C using SDL2. No GPU acceleration — everything is drawn pixel-by-pixel into a color buffer.

## What it does

Renders a rotating wireframe cube as a mesh of triangles. Each frame, vertices are rotated around all three axes, projected to 2D with perspective, and the triangle edges are drawn with a DDA line algorithm. Runs at 30 FPS.

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
  main.c       - game loop, setup, projection, and rendering
  display.c/h  - window management, color buffer, and drawing primitives (pixel, line, rect, triangle)
  vector.c/h   - vec2/vec3 types and rotation functions
  triangle.c/h - triangle_t (three projected 2D points)
  mesh.c/h     - mesh_t with dynamic vertex/face arrays; hardcoded cube data and loader
  array.c/h    - generic dynamic array (stretchy buffer) via array_push / array_length / array_free
```
