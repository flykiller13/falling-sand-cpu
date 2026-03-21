#ifndef FALLINGSAND_RENDERER_H
#define FALLINGSAND_RENDERER_H
#include <memory>

#include "shader.h"
#include "../simulation/simulation.h"

struct Color {
  uint8_t r, g, b, a;
};

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
  unsigned int VAO, VBO, EBO;
  std::unique_ptr<Shader> shader;
  unsigned int texture_id_;
  std::vector<uint32_t> pixels;
  // Pixel data array - Holds the colors that are passed to the quad

  const Color yellow = {255, 255, 0, 255};
  const Color blue = {27, 81, 255, 200};
  const Color grey = {149, 149, 149, 255};
  const Color gas = {42, 132, 24, 80};
  const Color black = {0, 0, 0, 255};
  const Color debug = {255, 0, 255, 255};
};

#endif //FALLINGSAND_RENDERER_H