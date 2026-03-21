# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release   # Windows
# or: make                          # Linux/macOS
```

Run from the build directory:
```bash
Release\FallingSand.exe  # Windows
./FallingSand             # Linux/macOS
```

## Linting & Formatting

```bash
# Format all source files
clang-format -i src/**/*.cpp include/FallingSand/**/*.h

# Static analysis (example)
clang-tidy src/app.cpp -- -I include
```

`.clang-format` uses LLVM style: 2-space indent, 80-column limit, right-aligned pointers.
`.clang-tidy` enables `clang-diagnostic-*` and `clang-analyzer-*` checks.

## Architecture

CPU-based falling sand simulation. All particle physics run on the CPU; the GPU is used only for rendering a texture quad.

**Main loop** (`app.cpp`): fixed timestep accumulator → Input → Simulation tick → Renderer → ImGui UI.

**Simulation** (`src/simulation/simulation.cpp`): Double-buffered 2D grid (read from `grid`, write to `next_grid` each tick). Iterates cells bottom-to-top to apply gravity. Per-cell movement rules:
- **Sand**: falls down, then diagonally down
- **Water**: falls down, spreads horizontally
- **Stone**: static
- **Gas**: rises up, spreads horizontally
- Random left/right choice each tick prevents directional bias artifacts.

**Grid** (`src/simulation/grid.cpp`): `std::vector<Cell>` with indexing `x + y * width`. Cell types: `Empty, Sand, Stone, Water, Gas`.

**Renderer** (`src/renderer/renderer.cpp`): Uploads a pixel buffer to a full-screen OpenGL texture each frame (`glTexSubImage2D`). Uses `GL_NEAREST` filtering for sharp pixels. Cell-to-RGBA color mapping lives here.

**Shader** (`src/renderer/shader.cpp`): Loads GLSL files from `shaders/`. `shader.vs` / `shader.fs` are a simple textured quad — no transforms, just UV lookup.

**Input** (`src/input.cpp`): Maps mouse position to grid coordinates, paints a circular brush of the selected cell type.

**UI** (`src/ui.cpp`): ImGui panel for brush type, brush size, and simulation stats.

## Dependencies

Bundled in-tree: GLAD (`include/glad/`, `src/glad.c`), ImGui (`include/imgui-master/`).
System dependencies (must be installed): OpenGL 4.3, GLFW3.

Default simulation grid: 400×400. The README notes it runs at 60 FPS on 1000×1000.