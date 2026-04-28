// GLAD download:
// https://glad.dav1d.de/generated/tmpk7onoi3rglad/

#include "falling-sand/renderer/renderer.h"

#include <cmath>
#include <filesystem>
#include <glad/glad.h> // must be included before GLFW
#include <GLFW/glfw3.h>

#include <iostream>

#include "falling-sand/renderer/shader.h"

Renderer::Renderer() {
}

void Renderer::init(const Simulation &sim) {
  // Build and compile shader program
  std::filesystem::path project_root = std::filesystem::current_path();
  while (!std::filesystem::exists(project_root / "shaders") && project_root.
         has_parent_path()) {
    project_root = project_root.parent_path();
  }
  std::cout << project_root << std::endl;
  std::filesystem::path shader_path = project_root / "shaders";
  std::string vertex_path = (shader_path / "shader.vs").string();
  std::string fragment_path = (shader_path / "shader.fs").string();
  shader_ = std::make_unique<
    Shader>(vertex_path.c_str(), fragment_path.c_str());

  constexpr float vertices[] = {
      // positions          // colors           // texture coords
      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
  };
  const unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3 // second triangle
  };

  // Create VAO
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_); // Bind vertex array object

  // Create VBO
  glGenBuffers(1, &vbo_);
  // Copy our vertices array in a buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create EBO
  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // tex coords attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Initialize pixel buffer
  pixels_.resize(sim.get_grid_width() * sim.get_grid_height());

  // Initialize texture
  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_2D, texture_id_);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Create texture - We update the texture pixels with the simulation grid and display
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sim.get_grid_width(),
               sim.get_grid_height(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, pixels_.data());

  // Generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  shader_->use();
}

void Renderer::render() {
  // clear color buffer
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw triangles
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  // Unbind VAO
  glBindVertexArray(0);
}

void Renderer::update(const Simulation &sim) {
  const std::vector<Cell> &cells = sim.get_cells();

  for (int i = 0; i < cells.size(); i++) {
    // Update pixel color to cell color
    Cell cell = cells[i];
    pixels_[i] = color_to_abgr(cell.color);
  }

  // Update texture
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sim.get_grid_width(),
                  sim.get_grid_height(), GL_RGBA, GL_UNSIGNED_BYTE,
                  pixels_.data());
}

void Renderer::cleanup() {
  glDeleteVertexArrays(1, &vao_);
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteTextures(1, &texture_id_);
}

uint32_t Renderer::color_to_abgr(const Color &color) {

  return (static_cast<uint32_t>(color.a << 24)
          | static_cast<uint32_t>(color.b << 16)
          | static_cast<uint32_t>(color.g << 8)
          | static_cast<uint32_t>(color.r));
}