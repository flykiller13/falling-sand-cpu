# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

CMake project (C++20, requires CMake 3.28+, OpenGL 4.3, GLFW3, pkg-config). Executable target is `falling_sand`.

```bash
mkdir build && cd build
cmake ..
cmake --build .
./falling_sand        # or Release\falling_sand.exe on Windows/MSVC
```

There is an existing `cmake-build-debug/` directory (CLion). No test suite, no linter beyond `.clang-format`.

When adding new `.cpp` files, they must be added explicitly to the `add_executable(falling_sand ...)` list in `CMakeLists.txt` — globbing is not used.

## Architecture

This is the **CPU branch** of a falling-sand simulator. The CPU runs the simulation; the GPU only blits a pixel buffer onto a fullscreen textured quad. A separate GPU branch exists.

### Core flow (per frame)
`App` (src/app.cpp) owns the GLFW window, `Simulation`, `Renderer`, `UI`, and `Input`, and drives the loop:
1. `Input` reads mouse/keyboard, paints cells into the `Simulation`'s grid via brush.
2. `Simulation::update()` advances the sim using **double-buffered grids** (`grid_` → `next_grid_`, then swap).
3. `Renderer` converts cells to an AGBR pixel array and uploads via `glTexSubImage2D` to a single texture quad.
4. `UI` (ImGui) overlays controls/stats.

Initialization is centralized through a `Config` struct (`include/falling-sand/config.h`) — window size, grid size, chunk size, dirty-margin all flow through it into `App`. Prefer extending `Config` over adding new constructor parameters.

### Simulation model
- `Grid` stores `Cell { CellType, ... }`. `CellType` densities drive **generic swap rules**: a higher-density cell may swap with a lower-density one (sand sinks in water, gas rises in liquid). Avoid hardcoding pairwise interactions — extend the density table instead.
- Movement rules per type live in `Simulation` (`src/simulation/simulation.cpp`): sand checks `down / down-dir / down-!dir`; water adds horizontal; gas mirrors water upward. `dir` is randomized per cell to prevent left-bias.
- **Iteration order matters**: left-to-right, bottom-to-top (y=0 is bottom). Changing this will produce visual artifacts unless the rules are also reworked.
- **Dirty-chunk system**: the grid is partitioned into chunks; only chunks containing recent activity (plus a configurable `margin` of neighbors) are iterated. This is what makes 1000×1000 viable. When adding new movement rules, ensure they mark affected chunks dirty (and wake neighbors when motion crosses chunk borders) or particles will appear to freeze.
- Randomness must stay on the CPU side of the design — the GPU branch uses a deterministic pull method, so don't share rule code assuming RNG is available there.

### Rendering
`Renderer` is intentionally thin: compile shaders (`shaders/shader.vs`/`.fs`), upload pixel buffer, draw quad. Cell→color mapping lives in `renderer/color.h`. There is no per-cell GPU work.

### Future-work hooks already discussed
- Multithreading the dirty-chunk update via **checkerboard pattern** to avoid neighbor races.
- Per-particle velocity/acceleration (easier on CPU than GPU branch).
- Loading `Config` from JSON.

## Repo notes
- `third-party/` vendors imgui and glad — do not edit; they are built as the static `imgui` lib.
- Headers live under `include/falling-sand/...` and are included as `<falling-sand/...>`.