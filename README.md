# Falling Sand Simulation

![Falling Sand Demo](resources/cpu_demo.gif)

*"From ashes to dust, from CPU to GPU"*

This is the first version of the falling sand simulation. It runs entirely on the **CPU**, using the GPU only to render
a texture quad on the screen. This allows us to run a **1000x1000 simulation** at 60 fps.

I've always been interested in falling sand simulations, but [Noita](https://noitagame.com/) finally inspired me to make
one.

> **Note**: This is the CPU branch. For the GPU-accelerated version using compute shaders, check out
> the [GPU branch](https://github.com/flykiller13/ron-falling-sand/tree/gpu).

## How It Works

### Cell Movement Rules

The rules for cell movement are fairly simple. **Sand** checks `down → down-left → down-right` and if one of these is
empty, it moves there (swaps). **Water** follows the same pattern but additionally checks `left` and `right`, giving it
that characteristic "filling up" effect. **Smoke/Gas** behaves like water but checks upward instead of downward.

```cpp
case CellType::Sand:
    if (can_move_to(x, y - 1)) {
        // Check below
        move_to(x, y, x, y - 1);
    } else {
        if (can_move_to(x + dir, y - 1)) {
            // Check left/right-down
            move_to(x, y, x + dir, y - 1);
        }
    }
    break;
```

### Density-Based Interactions

Interaction between cell types is evaluated using each cell type's **density**. A higher-density cell is allowed to swap
with a lower-density cell, which is what produces effects like **sand sinking through water** or **gas rising through
liquid**. This also lets us implement interactions generically from a single density value instead of hardcoding every
pairwise particle interaction.

```cpp
// Move is allowed within bounds and when density is lower
return is_in_bounds(x, y) &&
       (density(get_cell(x, y).type) < density(cell_type)) &&
       (density(next_grid_.get_cell(x, y).type) < density(cell_type));
```

### Random Direction Selection

One important mechanic is choosing a **random direction** (left/right) because cells check their neighbors sequentially
and their movement direction needs to be known. This gives the cells a more natural feel - without it, you get a bias
towards one side. Each cell randomly decides whether to check left or right first, creating that organic, chaotic
behavior you'd expect from falling sand.

```cpp
int dir = (dis(gen) == 0) ? -1 : 1; // Randomly choose left or right
```

This is interesting because on the [GPU branch](https://github.com/flykiller13/falling-sand-gpu), we can't use random
since we're using the pull
method - cell movement has to be deterministic.

### Iteration Order

When running the simulation, we iterate over each cell in the grid. The **iteration order matters** for correctness - we
process left to right, and for each column we process from bottom to top (where y=0 is the bottom). This ensures
particles fall naturally and prevents ordering artifacts. We use **double buffering** (reading from `grid`, writing to
`next_grid`) to ensure we always have consistent state during the update, then swap buffers after all cells are
processed. Without proper iteration order, you could get visual artifacts like particles teleporting or not falling
smoothly.

```cpp
// Iterate over grid - order matters!
for (int x = 0; x < grid.width; x++) {
    for (int y = 0; y < grid.height; y++) {
        Cell curr = getCell(x, y);
        // Process cell movement...
    }
}
// Swap buffers after all cells are processed
std::swap(grid.cells, next_grid.cells);
```

### Dirty Chunk System

When trying to run simulations at higher grid sizes (800x800 and up), the simulation was iterating over every cell in
the grid every tick, even when the simulation was empty or most particles had settled. That's 640,000 empty iterations
per tick. To solve this we use a **dirty chunk system**. We cut the grid into chunks - in my tests, a **16×16 grid of chunks on an
800x800 grid** (so 50×50 pixels per chunk) was a good compromise. If there is movement in a chunk, or close enough to a
neighboring chunk, the chunk is marked dirty. Each tick we iterate only over the dirty chunks. To wake up neighboring
chunks we use a **margin value**, which can be tweaked through the config.

This optimization massively increased performance, allowing us to run grids of **1000x1000 and beyond**. At higher
resolutions the sim becomes too small visually, and we can apply the same concept by only simulating a specific region
of the grid (and using the chunk system within it).

This system can be further enhanced by introducing **multithreading**. In that case we would have to iterate over the
chunks in a **checkerboard pattern** to avoid race conditions between neighboring chunks updating simultaneously.

*(Performance comparison table will be added later.)*

### Rendering Pipeline

The cells get translated to colors that are saved in a **pixel array**. Each cell type maps to a specific color (sand →
yellow, water → blue, stone → grey, etc.). The pixel array is then passed to the **texture quad**, which is displayed on
screen.

```cpp
// Convert cells to pixel colors
for (int i = 0; i < cells.size(); i++) {
    Cell cell = cells[i];
    switch (cell.type) {
        case CellType::Sand:
            pixels[i] = color_to_agbr(yellow);
            break;
        // ... other cell types
    }
}
// Update texture with pixel data
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, 
                GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
```

## Future Optimizations

- Dirt chunks have been implemented. The next step would be to multithread them.

- Since implementing rules is easier in this CPU version, we can add **velocity/acceleration** pretty easily. Might do
  that in the future also.

## Tested On

- **OS**: Windows 11
- **CPU**: AMD Ryzen 9 7900X 12-core
- **GPU**: NVIDIA GeForce RTX 4070 SUPER
- **RAM**: 64 GB

## Tech Stack

- **C++20** - Core language
- **OpenGL 4.3** - Graphics API (texture quad rendering)
- **GLFW** - Window management and input
- **ImGui** - User Interface
- **GLAD** - OpenGL loader
- **CMake** - Build system

## Installation

### Prerequisites

- **CMake** 3.28 or higher
- **C++20** compatible compiler (GCC, Clang, or MSVC)
- **OpenGL 4.3** compatible graphics driver
- **GLFW3** development libraries
- **pkg-config** (for finding GLFW)

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake libglfw3-dev pkg-config

# Build
mkdir build && cd build
cmake ..
cmake --build .

# Run
./falling_sand
```

### Windows (MSVC)

```bash
# Install GLFW manually or via vcpkg
# Then build:
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

# Run
Release\falling_sand.exe
```

## Project Structure

```
falling-sand-cpu/
├── include/
│   └── falling-sand/
│       ├── app.h              # Main application class
│       ├── config.h           # Configuration struct (window/grid/chunk params)
│       ├── input.h            # Input handling (mouse/keyboard)
│       ├── ui.h               # ImGui interface
│       ├── renderer/
│       │   ├── renderer.h     # OpenGL rendering (texture quad)
│       │   ├── color.h        # Cell-type → color mapping
│       │   └── shader.h       # Shader compilation
│       └── simulation/
│           ├── simulation.h   # CPU simulation logic
│           └── grid.h         # Grid representation
├── src/
│   └── [corresponding .cpp files + main.cpp]
├── shaders/
│   ├── shader.vs              # Vertex shader (texture quad)
│   └── shader.fs              # Fragment shader (texture sampling)
├── third-party/               # Vendored ImGui and GLAD (built as static lib)
└── CMakeLists.txt
```

### Key Classes

- **`App`** - Main application loop, manages GLFW window, coordinates all subsystems
- **`Simulation`** - Manages the CPU-side simulation, handles double buffering, implements cell movement rules, chunk
  system
- **`Renderer`** - Handles OpenGL rendering, converts cells to pixel array, uploads to texture quad
- **`Grid`** - Grid representation storing cell data
- **`UI`** - ImGui interface for brush selection, controls and stats
- **`Input`** - Processes mouse/keyboard input for drawing particles

## Configuration

The application is initialized from a `Config` struct that bundles window size, grid size, and chunk parameters into a
single object passed to `App`. This keeps initialization parameters in one place instead of scattered across
constructors. In the future this may be expanded to load from a JSON file so settings can be changed without rebuilding.

## Controls

- **Left mouse** — place the selected particle
- **Right mouse** — place stone
- **Top-left UI** — brush options, particle selection, and sim stats

<!-- TODO: insert UI screenshot here -->

## Current Status

The simulation currently supports:

- **Sand** - Falls down and to the sides
- **Water** - Flows horizontally when it can't fall
- **Stone** - Static walls
- **Gas** - Rises up and spreads horizontally

The grid is configured to 400x400 by default but can handle up to 1000x1000.

## Future Improvements

- [ ] Add velocity/acceleration to particles
- [ ] Add more particle types (fire, acid, etc.)

## License

This project is licensed under the [MIT License](LICENSE). Vendored dependencies in `third-party/` (Dear ImGui, GLAD) retain their own licenses; GLFW (linked at build time) is distributed under the zlib/libpng license.
