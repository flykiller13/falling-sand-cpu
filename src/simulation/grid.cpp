#include "falling-sand/simulation/grid.h"

Grid::Grid(int width, int height) : width(width), height(height) {
  // Initialize the grid with empty cells
  for (int i = 0; i < width * height; i++) {
    cells.push_back(Cell(CellType::Empty));
  }
}

void Grid::set_cell(int x, int y, CellType type) {
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;

  cells[x + y * width] = Cell(type);
}