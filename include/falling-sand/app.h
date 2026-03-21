#ifndef FALLINGSAND_APP_H
#define FALLINGSAND_APP_H

#include "renderer/renderer.h"
#include "input.h"
#include "ui.h"
#include "simulation/simulation.h"

#define ENGINE_TITLE "Falling Sand"

class App {
public:
  App();
  bool init(); // Initializes GLFW window and simulation objects
  void run(); // Runs the main simulation loop
  void cleanup(); // Terminates GLFW window and simulation objects

private:
  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  // Updates the viewport on window resize

  Input input;
  Simulation sim;
  Renderer renderer;
  UI ui;

  GLFWwindow *window_;
  int window_width;
  int window_height;
  double last_frame = 0.0;
  double current_frame, delta_time;
};


#endif //FALLINGSAND_APP_H