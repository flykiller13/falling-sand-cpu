#ifndef FALLINGSAND_GRID_H
#define FALLINGSAND_GRID_H
#include "falling-sand/renderer/color.h"

#include <vector>

enum class CellType {
  Empty,
  Sand,
  Stone,
  Water,
  Gas,
};

struct Cell {
  CellType type;
  Color color; // cells store their own color that includes an offset
};

class Grid {
public:
  Grid(int width, int height);
  void set_cell(int x, int y, CellType type);
  // Generates a new cell of type CellType at x, y
  void set_cell(int x, int y, Cell cell); // Places the passed cell at x, y
  Cell get_cell(int x, int y) const { return cells[x + y * width]; }

  int width, height;
  std::vector<Cell> cells;

private:
  Color get_type_color(CellType type);

  const Color yellow = {255, 255, 0, 255};
  const Color blue = {27, 81, 255, 200};
  const Color grey = {149, 149, 149, 255};
  const Color gas = {42, 132, 24, 80};
  const Color black = {0, 0, 0, 255};
  const Color debug = {255, 0, 255, 255};
};


#endif //FALLINGSAND_GRID_H