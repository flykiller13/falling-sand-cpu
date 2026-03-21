#ifndef FALLINGSAND_SIMULATION_H
#define FALLINGSAND_SIMULATION_H
#include "grid.h"

#include <random>

class Simulation {
public:
  Simulation(int sim_width, int sim_height);
  void update(double delta_time);
  // Runs ticks of the simulation based on delta time
  void simulation_tick();
  void clear(); // Clears the sim - Sets all cells to empty

  int get_grid_width() const { return grid.width; }
  int get_grid_height() const { return grid.height; }

  const std::vector<Cell> &get_cells() const { return grid.cells; }

  void set_cell(int x, int y, CellType type) { grid.set_cell(x, y, type); }

  const Cell &getCell(int x, int y) const {
    return grid.cells[x + y * grid.width];
  }

  const uint32_t &get_active_cell_count() const { return active_cell_count; }

  bool is_in_bounds(int x, int y) const;
  bool can_move_to(int x, int y) const; // Returns true if the cell is empty
  void move_to(int fromX, int fromY, int toX, int toY); // Swaps cells

private:
  // We use a double buffer method - Data is read from grid and written to next_grid
  Grid grid;
  Grid next_grid;
  uint32_t active_cell_count = 0;

  double accumulator = 0.0;
  const double fixed_delta_time = 1.0 / 60; // 60 ticks per second
  const double MAX_DELTA_TIME = 1.0 / 10.0;

  // random - for choosing a direction
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_int_distribution<> dis;
};

#endif // FALLINGSAND_SIMULATION_H