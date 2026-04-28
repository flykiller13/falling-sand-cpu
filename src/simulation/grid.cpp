#include "falling-sand/simulation/grid.h"

#include <algorithm>
#include <random>

static std::mt19937 rng(std::random_device{}());
static std::uniform_int_distribution<int> dist(-10, 10);


Grid::Grid(int width, int height) : width(width), height(height) {
  // Initialize the grid with empty cells
  for (int i = 0; i < width * height; i++) {
    cells.push_back(Cell(CellType::Empty, get_type_color(CellType::Empty)));
  }
}

void Grid::set_cell(int x, int y, CellType type) {
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;

  Color cell_color = get_type_color(type);
  cells[x + y * width] = Cell(type, cell_color);
}

void Grid::set_cell(int x, int y, Cell cell) {
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;

  cells[x + y * width] = cell;
}

Color Grid::get_type_color(CellType type) {
  Color c{};
  switch (type) {
  case CellType::Sand:
    c = yellow;
    break;
  case CellType::Water:
    c = blue;
    break;
  case CellType::Gas:
    c = gas;
    break;
  case CellType::Stone:
    c = grey;
    break;
  case CellType::Empty:
    c = black;
    break;
  default: // debug color is used to see the particles
    c = debug;
    break;
  }

  if (type != CellType::Empty) {
    // Add random offsets
    c.r = std::clamp(static_cast<int>(c.r) + dist(rng), 0, 255);
    c.g = std::clamp(static_cast<int>(c.g) + dist(rng), 0, 255);
    c.b = std::clamp(static_cast<int>(c.b) + dist(rng), 0, 255);
  }

  return c;
}