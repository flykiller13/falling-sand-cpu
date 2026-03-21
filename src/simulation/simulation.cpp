#include "falling-sand/simulation/simulation.h"

#include <random>

Simulation::Simulation(int sim_width, int sim_height)
  : grid(sim_width, sim_height), next_grid(sim_width, sim_height), gen(rd()),
    dis(0, 1) {
}

void Simulation::update(double delta_time) {
  delta_time = std::min(delta_time, MAX_DELTA_TIME);
  accumulator += delta_time;

  // Tick only in fixed time steps
  while (accumulator >= fixed_delta_time) {
    accumulator -= fixed_delta_time;
    simulation_tick();
  }
}

void Simulation::simulation_tick() {
  // Copy current to next
  for (int x = 0; x < grid.width; x++) {
    for (int y = 0; y < grid.height; y++) {
      next_grid.set_cell(x, y, getCell(x, y).type);
    }
  }

  active_cell_count = 0;

  // Iterate over grid
  for (int x = 0; x < grid.width; x++) {
    for (int y = 0; y < grid.height; y++) {
      Cell curr = getCell(x, y);

      if (curr.type != CellType::Empty)
        active_cell_count++;

      int dir = (dis(gen) == 0) ? -1 : 1; // Randomly choose left or right

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

      case CellType::Gas: // Farts(Smoke) is the same as water but goes up
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
  std::swap(grid.cells, next_grid.cells);
}

void Simulation::clear() {
  for (int x = 0; x < grid.width; x++) {
    for (int y = 0; y < grid.height; y++) {
      grid.set_cell(x, y, CellType::Empty);
    }
  }
}

bool Simulation::is_in_bounds(int x, int y) const {
  return x >= 0 && x < grid.width && y >= 0 && y < grid.height;
}

bool Simulation::can_move_to(int x, int y) const {
  // Need to check next grid too since we write only to next
  return is_in_bounds(x, y) && (getCell(x, y).type == CellType::Empty) &&
         (next_grid.get_cell(x, y).type == CellType::Empty);
}

void Simulation::move_to(int fromX, int fromY, int toX, int toY) {
  if (!is_in_bounds(fromX, fromY) || !is_in_bounds(toX, toY))
    return;

  CellType fromType = getCell(fromX, fromY).type;
  CellType toType = getCell(toX, toY).type;

  next_grid.set_cell(toX, toY, fromType);
  next_grid.set_cell(fromX, fromY, toType);
}