#include "falling-sand/app.h"
#include <glad/glad.h> // must be included before GLFW
#include <GLFW/glfw3.h>

#include <iostream>

#include "falling-sand/input.h"

App::App() : input_(), sim_(800, 800), renderer_(), ui_(), window_(nullptr),
             window_width_(800), window_height_(800), current_frame_(0),
             delta_time_(0) {
}

App::App(const Config &config) : input_(),
                                 sim_(config.grid_width, config.grid_height),
                                 renderer_(),
                                 ui_(),
                                 window_(nullptr),
                                 window_width_(config.window_width),
                                 window_height_(config.window_height),
                                 current_frame_(0),
                                 delta_time_(0), config_(config) {
}

App::~App() {
  ui_.terminate();
  renderer_.cleanup();

  // Terminate GLFW
  glfwDestroyWindow(window_);
  glfwTerminate();
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
  window_ = glfwCreateWindow(window_width_, window_height_, config_.title,
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
  glViewport(0, 0, window_width_, window_height_);
  // Set window re-size callback
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

  // Initialize
  renderer_.init(sim_);
  input_.init(window_);
  ui_.init(window_);

  return true;
}

void App::run() {
  last_frame_ = glfwGetTime();

  // Render loop
  while (!glfwWindowShouldClose(window_)) {

    current_frame_ = glfwGetTime();
    delta_time_ = current_frame_ - last_frame_;
    last_frame_ = current_frame_;

    input_.update(sim_, ui_.brush_type, ui_.brush_size);
    sim_.update(ui_.paused ? 0.0f : delta_time_);
    renderer_.update(sim_);
    renderer_.render();
    ui_.update(sim_);

    // Swap buffers and poll IO events
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void App::cleanup() {

}

void App::framebuffer_size_callback(GLFWwindow *window, int width,
                                    int height) {
  glViewport(0, 0, width, height);

  App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
  if (app) {
    app->window_width_ = width;
    app->window_height_ = height;
  }
}