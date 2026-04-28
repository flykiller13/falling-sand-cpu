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

  int get_grid_width() const { return grid_.width; }
  int get_grid_height() const { return grid_.height; }

  const std::vector<Cell> &get_cells() const { return grid_.cells; }

  void set_cell(int x, int y, CellType type) { grid_.set_cell(x, y, type); }

  const Cell &get_cell(int x, int y) const {
    return grid_.cells[x + y * grid_.width];
  }

  const uint32_t &get_active_cell_count() const { return active_cell_count_; }

  bool is_in_bounds(int x, int y) const;
  bool can_move_to(int x, int y) const; // Returns true if the cell is empty
  void move_to(int from_x, int from_y, int to_x, int to_y); // Swaps cells

private:
  // We use a double buffer method - Data is read from grid and written to next_grid
  Grid grid_;
  Grid next_grid_;
  uint32_t active_cell_count_ = 0;

  double accumulator_ = 0.0;
  const double fixed_delta_time = 1.0 / 60; // 60 ticks per second
  const double max_delta_time = 1.0 / 10.0;

  // random - for choosing a direction
  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_int_distribution<> dis_;
};

#endif // FALLINGSAND_SIMULATION_H