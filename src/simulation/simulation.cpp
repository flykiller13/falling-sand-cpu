#include "falling-sand/simulation/simulation.h"

#include <random>

Simulation::Simulation(int sim_width, int sim_height)
  : grid_(sim_width, sim_height), next_grid_(sim_width, sim_height),
    gen_(rd_()),
    dis_(0, 1) {
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
  // Copy current grid to next grid
  for (int x = 0; x < grid_.width; x++) {
    for (int y = 0; y < grid_.height; y++) {
      next_grid_.set_cell(x, y, get_cell(x, y));
    }
  }

  active_cell_count_ = 0;

  // Iterate over grid
  for (int x = 0; x < grid_.width; x++) {
    for (int y = grid_.height - 1; y >= 0; y--) {
      // Iterate in reverse so we dont iterate multiple times over falling particles
      Cell curr = get_cell(x, y);

      if (curr.type != CellType::Empty)
        active_cell_count_++;

      int dir = (dis_(gen_) == 0) ? -1 : 1; // Randomly choose left or right

      // MOVEMENT RULES
      switch (curr.type) {
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

      case CellType::Water: // Water is the same as sand but we also
        // check left and right
        if (can_move_to(x, y - 1)) {
          // Check below
          move_to(x, y, x, y - 1);
        } else {
          if (can_move_to(x + dir, y - 1)) {
            // Check left/right-down
            move_to(x, y, x + dir, y - 1);
          } else {
            if (can_move_to(x + dir, y)) {
              // Check left/right
              move_to(x, y, x + dir, y);
            }
          }
        }
        break;

      case CellType::Gas: // Gas is the same as water but goes up
        if (can_move_to(x, y + 1)) {
          // Check above
          move_to(x, y, x, y + 1);
        } else {
          if (can_move_to(x + dir, y + 1)) {
            // Check left/right-up
            move_to(x, y, x + dir, y + 1);
          } else {
            if (can_move_to(x + dir, y)) {
              // Check left/right
              move_to(x, y, x + dir, y);
            }
          }
        }
        break;

      default:
        break;
      }
    }
  }

  // swap buffers
  std::swap(grid_.cells, next_grid_.cells);
}

void Simulation::clear() {
  for (int x = 0; x < grid_.width; x++) {
    for (int y = 0; y < grid_.height; y++) {
      grid_.set_cell(x, y, CellType::Empty);
      next_grid_.set_cell(x, y, CellType::Empty);
    }
  }
}

bool Simulation::is_in_bounds(int x, int y) const {
  return x >= 0 && x < grid_.width && y >= 0 && y < grid_.height;
}

bool Simulation::can_move_to(int x, int y) const {
  // Need to check next grid too since we write only to next
  return is_in_bounds(x, y) && (get_cell(x, y).type == CellType::Empty) &&
         (next_grid_.get_cell(x, y).type == CellType::Empty);
}

void Simulation::move_to(int from_x, int from_y, int to_x, int to_y) {
  if (!is_in_bounds(from_x, from_y) || !is_in_bounds(to_x, to_y))
    return;

  next_grid_.set_cell(to_x, to_y, get_cell(from_x, from_y));
  next_grid_.set_cell(from_x, from_y, get_cell(to_x, to_y));
}