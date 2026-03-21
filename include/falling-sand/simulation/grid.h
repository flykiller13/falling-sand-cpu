#ifndef FALLINGSAND_GRID_H
#define FALLINGSAND_GRID_H
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
};

class Grid {
public:
  Grid(int width, int height);
  void set_cell(int x, int y, CellType type);
  Cell get_cell(int x, int y) const { return cells[x + y * width]; }

  int width, height;
  std::vector<Cell> cells;
};


#endif //FALLINGSAND_GRID_H