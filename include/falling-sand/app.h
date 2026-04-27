#ifndef FALLINGSAND_APP_H
#define FALLINGSAND_APP_H

#include "config.h"
#include "renderer/renderer.h"
#include "input.h"
#include "ui.h"
#include "simulation/simulation.h"

#define ENGINE_TITLE "Falling Sand"

/**
 *
 */
class App {
public:
  App();
  App(const Config &config = Config{});
  ~App();

  bool init(); // Initializes GLFW window and simulation objects
  void run(); // Runs the main simulation loop
  void cleanup(); // Terminates GLFW window and simulation objects

private:
  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  // Updates the viewport on window resize

  Input input_;
  Simulation sim_;
  Renderer renderer_;
  UI ui_;

  GLFWwindow *window_{};
  int window_width_{};
  int window_height_{};

  double last_frame_ = 0.0;
  double current_frame_{}, delta_time_{};

  Config config_;
};


#endif //FALLINGSAND_APP_H