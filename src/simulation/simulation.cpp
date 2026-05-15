#include "falling-sand/simulation/simulation.h"

#include <algorithm>
#include <random>

Simulation::Simulation(const Config &config)
  : grid_(config.grid_width, config.grid_height),
    total_cell_count(config.grid_width * config.grid_height),
    next_grid_(config.grid_width, config.grid_height),
    num_chunks_x(config.num_chunks_x),
    num_chunks_y(config.num_chunks_y),
    total_chunk_count(config.num_chunks_x * config.num_chunks_y),
    chunk_size_(config.grid_width / config.num_chunks_x),
    chunk_margin_(config.chunk_margin),
    gen_(rd_()),
    dis_(0, 1) {

  // initialize chunk array - start dirty so all chunks are processed on first pass
  chunks_.resize(num_chunks_x * num_chunks_y, Chunk{true, false});
}

void Simulation::update(double delta_time) {
  delta_time = std::min(delta_time, max_delta_time);
  accumulator_ += delta_time;

  // Tick only in fixed time steps
  while (accumulator_ >= fixed_delta_time) {
    accumulator_ -= fixed_delta_time;
    simulation_tick();
  }
}

void Simulation::simulation_tick() {
  // Copy current grid to next grid - only dirty chunks
  for (int cx = 0; cx < num_chunks_x; cx++) {
    for (int cy = 0; cy < num_chunks_y; cy++) {
      if (chunks_[chunk_index(cx, cy)].dirty_current) {
        int x0 = cx * chunk_size_;
        int x1 = std::min((cx + 1) * chunk_size_, grid_.width);
        int y0 = cy * chunk_size_;
        int y1 = std::min((cy + 1) * chunk_size_, grid_.height);
        for (int x = x0; x < x1; x++) {
          for (int y = y0; y < y1; y++) {
            next_grid_.set_cell(x, y, get_cell(x, y));
          }
        }
      }
    }
  }

  active_chunk_count_ = 0;
  active_cell_count_ = 0;
  for (int cx = 0; cx < num_chunks_x; cx++) {
    for (int cy = 0; cy < num_chunks_y; cy++) {
      if (chunks_[chunk_index(cx, cy)].dirty_current) {
        active_chunk_count_++;
        iterate_chunk(cx, cy);
      }
    }
  }

  // swap buffers
  std::swap(grid_.cells, next_grid_.cells);

  // swap chunk dirty
  for (int cx = 0; cx < num_chunks_x; cx++) {
    for (int cy = 0; cy < num_chunks_y; cy++) {
      Chunk &chunk = chunks_[chunk_index(cx, cy)];
      chunk.dirty_current = chunk.dirty_next;
      chunk.dirty_next = false;
    }
  }
}

void Simulation::clear() {
  for (int x = 0; x < grid_.width; x++) {
    for (int y = 0; y < grid_.height; y++) {
      grid_.set_cell(x, y, CellType::Empty);
      next_grid_.set_cell(x, y, CellType::Empty);
    }
  }
}

void Simulation::set_cell(int x, int y, CellType type) {
  grid_.set_cell(x, y, type);
  mark_dirty(x, y);
}

bool Simulation::is_in_bounds(int x, int y) const {
  return x >= 0 && x < grid_.width && y >= 0 && y < grid_.height;
}

bool Simulation::can_move_to(int x, int y, CellType cell_type) const {
  // Move is allowed within bounds and when density is lower
  return is_in_bounds(x, y) &&
         (density(get_cell(x, y).type) < density(cell_type)) &&
         (density(next_grid_.get_cell(x, y).type) < density(cell_type));
}

void Simulation::move_to(int from_x, int from_y, int to_x, int to_y) {
  if (!is_in_bounds(from_x, from_y) || !is_in_bounds(to_x, to_y))
    return;

  if (next_grid_.get_cell(from_x, from_y).type == grid_.get_cell(from_x, from_y)
      .type) {
    // Was the particle displaced?
    next_grid_.set_cell(to_x, to_y, get_cell(from_x, from_y));
    next_grid_.set_cell(from_x, from_y, get_cell(to_x, to_y));

    // mark chunks dirty
    mark_dirty(from_x, from_y);
    mark_dirty(to_x, to_y);
  }

}

float Simulation::density(CellType type) {
  switch (type) {
  case CellType::Sand:
    return 50.0f;
  case CellType::Water:
    return 1.0f;
  case CellType::Gas:
    return 0.1f;
  case CellType::Stone:
    return 100.f;
  default: // empty and default density is 0
    return 0.0f;
  }
}

void Simulation::mark_dirty(const int x, const int y) {
  // chunk indices
  const int cx = x / chunk_size_;
  const int cy = y / chunk_size_;

  chunks_[chunk_index(cx, cy)].dirty_next = true;

  // check neighboring chunks
  if ((cx + 1) < num_chunks_x && x >= (cx + 1) * chunk_size_ - chunk_margin_)
    // right neighbor
    chunks_[chunk_index(cx + 1, cy)].dirty_next = true;

  if ((cx - 1) >= 0 && x <= cx * chunk_size_ + chunk_margin_) // left neighbor
    chunks_[chunk_index(cx - 1, cy)].dirty_next = true;

  if ((cy + 1) < num_chunks_y && y >= (cy + 1) * chunk_size_ - chunk_margin_)
    // top neighbor
    chunks_[chunk_index(cx, cy + 1)].dirty_next = true;

  if ((cy - 1) >= 0 && y <= cy * chunk_size_ + chunk_margin_)
    // bottom neighbor
    chunks_[chunk_index(cx, cy - 1)].dirty_next = true;
}

void Simulation::iterate_chunk(int cx, int cy) {

  int start_x = std::max(cx * chunk_size_, 0);
  int end_x = std::min((cx + 1) * chunk_size_, grid_.width);
  int start_y = std::max(cy * chunk_size_, 0);
  int end_y = std::min((cy + 1) * chunk_size_, grid_.height);

  for (int x = start_x; x < end_x; x++) {
    for (int y = start_y; y < end_y; y++) {
      Cell curr = get_cell(x, y);

      if (curr.type != CellType::Empty)
        active_cell_count_++;

      int dir = (dis_(gen_) == 0) ? -1 : 1; // Randomly choose left or right
      CellType type;

      // MOVEMENT RULES
      switch (curr.type) {
      case CellType::Sand:
        type = CellType::Sand;
        if (can_move_to(x, y - 1, type)) {
          // Check below
          move_to(x, y, x, y - 1);
        } else {
          if (can_move_to(x + dir, y - 1, type)) {
            // Check left/right-down
            move_to(x, y, x + dir, y - 1);
          }
        }
        break;

      case CellType::Water: // Water is the same as sand but we also
        // check left and right
        type = CellType::Water;
        if (can_move_to(x, y - 1, type)) {
          move_to(x, y, x, y - 1);
        } else if (can_move_to(x + dir, y - 1, type)) {
          move_to(x, y, x + dir, y - 1);
        } else {
          // Spread horizontally up to dispersion_range cells
          for (int i = 1; i <= dispersion_range; i++) {
            if (!is_in_bounds(x + dir * i, y))
              break;
            if (can_move_to(x + dir * i, y, type)) {
              move_to(x, y, x + dir * i, y);
              break;
            }
          }
        }
        break;

      case CellType::Gas: // Gas is the same as water but goes up
        type = CellType::Gas;
        if (can_move_to(x, y + 1, type)) {
          move_to(x, y, x, y + 1);
        } else if (can_move_to(x + dir, y + 1, type)) {
          move_to(x, y, x + dir, y + 1);
        } else {
          for (int i = 1; i <= dispersion_range; i++) {
            if (!is_in_bounds(x + dir * i, y))
              break;
            if (can_move_to(x + dir * i, y, type)) {
              move_to(x, y, x + dir * i, y);
              break;
            }
          }
        }
        break;

      default:
        break;
      }
    }
  }
}