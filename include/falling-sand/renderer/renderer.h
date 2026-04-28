#ifndef FALLINGSAND_RENDERER_H
#define FALLINGSAND_RENDERER_H
#include "color.h"

#include <memory>

#include "shader.h"
#include "falling-sand/simulation/simulation.h"


class Renderer {
public:
  Renderer();
  void init(const Simulation &sim);
  // Initializes shader, VAO, VBO, EBO, Texture quad and pixel array
  // TODO separate
  void render(); // Renders the quad
  void update(const Simulation &sim);
  // Updates the pixel array and the texture quad with the simulation data
  void cleanup(); // Deletes gl objects

  uint32_t color_to_abgr(const Color &color);
  // Converts color (RGBA) to int32 (ABGR)

private:
  unsigned int vao_, vbo_, ebo_;
  std::unique_ptr<Shader> shader_;
  unsigned int texture_id_;
  std::vector<uint32_t> pixels_;
  // Pixel data array - Holds the colors that are passed to the quad
};

#endif //FALLINGSAND_RENDERER_H