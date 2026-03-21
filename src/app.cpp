#include "falling-sand/app.h"
#include <glad/glad.h> // must be included before GLFW
#include <GLFW/glfw3.h>

#include <iostream>

#include "falling-sand/input.h"

void App::framebuffer_size_callback(GLFWwindow *window, int width,
                                    int height) {
  glViewport(0, 0, width, height);

  App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->window_width = width;
    app->window_height = height;
  }
}

App::App() : input(), sim(400, 400), renderer(), ui(), window_(nullptr),
             window_width(800), window_height(800), current_frame(0),
             delta_time(0) {
}

bool App::init() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a window and its OpenGL context
  window_ = glfwCreateWindow(window_width, window_height, "Falling Sand",
                             nullptr, nullptr);
  if (window_ == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window_);

  glfwSetWindowUserPointer(window_, this);

  // Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return false;
  }

  // Set viewport
  glViewport(0, 0, window_width, window_height);
  // Set window re-size callback
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

  // Initialize
  renderer.init(sim);
  input.init(window_);
  ui.init(window_);

  return true;
}

void App::run() {
  last_frame = glfwGetTime();

  // Render loop
  while (!glfwWindowShouldClose(window_)) {

    current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    input.update(sim, ui.brush_type, ui.brush_size);
    sim.update(delta_time);
    renderer.update(sim);
    renderer.render();
    ui.update(sim);

    // Swap buffers and poll IO events
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void App::cleanup() {
  ui.terminate();
  renderer.cleanup();

  // Terminate GLFW
  glfwDestroyWindow(window_);
  glfwTerminate();
}