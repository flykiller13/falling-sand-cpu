#ifndef FALLINGSAND_SIMULATION_H
#define FALLINGSAND_SIMULATION_H
#include "grid.h"
#include "falling-sand/config.h"

#include <random>

struct Chunk {
  bool dirty_current; // process this tick
  bool dirty_next; // process next tick
};

class Simulation {
public:
  explicit Simulation(const Config &config);
  void update(double delta_time);
  // Runs ticks of the simulation based on delta time
  void simulation_tick();
  void clear(); // Clears the sim - Sets all cells to empty

  int get_grid_width() const { return grid_.width; }
  int get_grid_height() const { return grid_.height; }

  const std::vector<Cell> &get_cells() const { return grid_.cells; }

  void set_cell(int x, int y, CellType type);

  const Cell &get_cell(int x, int y) const {
    return grid_.cells[x + y * grid_.width];
  }

  const int get_active_cell_count() const { return active_cell_count_; }
  const int get_active_chunk_count() const { return active_chunk_count_; }
  const int get_num_chunks_x() const { return num_chunks_x; }
  const int get_num_chunks_y() const { return num_chunks_y; }

  bool is_in_bounds(int x, int y) const;
  bool can_move_to(int x, int y, CellType cell_type) const;
  void move_to(int from_x, int from_y, int to_x, int to_y); // Swaps cells

  const int total_cell_count;
  const int total_chunk_count;

private:
  static float density(CellType type); // returns the density of type
  void mark_dirty(int x, int y); // marks a chunk dirty given pixel coords
  void iterate_chunk(int cx, int cy);
  // iterates over the cells in chunks[cx][cy]
  [[nodiscard]] int chunk_index(const int cx, const int cy) const {
    return cy * num_chunks_x + cx;
  }

  // We use a double buffer method - Data is read from grid and written to next_grid
  Grid grid_;
  Grid next_grid_;
  int active_cell_count_ = 0;

  // dirty chunks - the grid is divided into chunks.
  // we iterate only over dirty chunks,
  // thus saving many iterations on settled particles
  const int num_chunks_x;
  const int num_chunks_y;
  std::vector<Chunk> chunks_;
  int chunk_size_;
  int chunk_margin_;
  int active_chunk_count_ = 0;

  // timestep
  double accumulator_ = 0.0;
  const double fixed_delta_time = 1.0 / 60; // 60 ticks per second
  const double max_delta_time = 1.0 / 10.0;
  const int dispersion_range = 2;
  // how many cells water/gas spread sideways per tick

  // random - for choosing a direction
  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_int_distribution<> dis_;
};

#endif // FALLINGSAND_SIMULATION_H