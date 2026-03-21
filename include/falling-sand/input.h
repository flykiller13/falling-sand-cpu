#ifndef FALLINGSAND_INPUT_H
#define FALLINGSAND_INPUT_H
#include <GLFW/glfw3.h>

#include "simulation/simulation.h"


class Input {
public:
  Input();
  void init(GLFWwindow *glfw_window); // Sets the GLFW window
  void update(Simulation &sim, CellType brushType, int brushSize);
  // Processes inputs
  void apply_brush(GLFWwindow *window, Simulation &sim, int brush_size,
                   CellType type);
  // Draws the chosen cell type in the simulation

private:
  GLFWwindow *window_;
};


#endif //FALLINGSAND_INPUT_H