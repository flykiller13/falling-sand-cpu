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
  std::filesystem::path projectRoot = std::filesystem::current_path();
  while (!std::filesystem::exists(projectRoot / "shaders") && projectRoot.
         has_parent_path()) {
    projectRoot = projectRoot.parent_path();
  }
  std::cout << projectRoot << std::endl;
  std::filesystem::path shaderPath = projectRoot / "shaders";
  std::string vertexPath = (shaderPath / "shader.vs").string();
  std::string fragmentPath = (shaderPath / "shader.fs").string();
  shader = std::make_unique<Shader>(vertexPath.c_str(), fragmentPath.c_str());

  float vertices[] = {
      // positions          // colors           // texture coords
      1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3 // second triangle
  };

  // Create VAO
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO); // Bind vertex array object

  // Create VBO
  glGenBuffers(1, &VBO);
  // Copy our vertices array in a buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Create EBO
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // tex coords attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Initialize pixel buffer
  pixels.resize(sim.get_grid_width() * sim.get_grid_height());

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
               0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

  // Generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  shader->use();
}

void Renderer::render() {
  // clear color buffer
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw triangles
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  // Unbind VAO
  glBindVertexArray(0);
}

void Renderer::update(const Simulation &sim) {
  const std::vector<Cell> &cells = sim.get_cells();

  for (int i = 0; i < cells.size(); i++) {
    Cell cell = cells[i];
    switch (cell.type) {
    case CellType::Sand:
      pixels[i] = color_to_abgr(yellow);
      break;
    case CellType::Water:
      pixels[i] = color_to_abgr(blue);
      break;
    case CellType::Gas:
      pixels[i] = color_to_abgr(gas);
      break;
    case CellType::Stone:
      pixels[i] = color_to_abgr(grey);
      break;
    case CellType::Empty:
      pixels[i] = color_to_abgr(black);
      break;
    default: // debug color is used to see the particles
      pixels[i] = color_to_abgr(debug);
      break;

    }
  }

  // Update texture
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sim.get_grid_width(),
                  sim.get_grid_height(), GL_RGBA, GL_UNSIGNED_BYTE,
                  pixels.data());
}

void Renderer::cleanup() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteTextures(1, &texture_id_);
}

uint32_t Renderer::color_to_abgr(const Color &color) {

  return (static_cast<uint32_t>(color.a << 24)
          | static_cast<uint32_t>(color.b << 16)
          | static_cast<uint32_t>(color.g << 8)
          | static_cast<uint32_t>(color.r));
}