#include "falling-sand/input.h"
#include "falling-sand/simulation/grid.h" // For CellType
#include "../third-party/imgui/imgui.h"

Input::Input() : window_(nullptr) {
}

void Input::init(GLFWwindow *glfw_window) {
  window_ = glfw_window;
}

void Input::update(Simulation &sim, CellType brushType, int brushSize) {
  int width, height;
  glfwGetWindowSize(window_, &width, &height);

  // ESC - Close window
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window_, true);

  // LMB - Draw selected material
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    apply_brush(window_, sim, brushSize, brushType);
  // RMB - Draw Stone
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    apply_brush(window_, sim, brushSize, CellType::Stone);
}

void Input::apply_brush(GLFWwindow *window, Simulation &sim, int brush_size,
                        CellType type) {
  // Check if the mouse is over an ImGui window
  // If it is, we don't want to draw pixels behind the menu
  if (ImGui::GetIO().WantCaptureMouse)
    return;

  // Calculate cursor position in sim grid
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  double x_ratio = static_cast<double>(width) / static_cast<double>(sim.
                     get_grid_width());
  double y_ratio = static_cast<double>(height) / static_cast<double>(sim.
                     get_grid_height());
  int grid_x = static_cast<int>(xpos / x_ratio);
  int grid_y = static_cast<int>((height - ypos) / y_ratio);

  // Paint circle around the cursor position
  for (int x = grid_x - brush_size; x <= grid_x + brush_size; x++) {
    for (int y = grid_y - brush_size; y <= grid_y + brush_size; y++) {
      if ((x - grid_x) * (x - grid_x) + (y - grid_y) * (y - grid_y) <=
          brush_size * brush_size)
        sim.set_cell(x, y, type);
    }
  }
}