#ifndef FALLING_SAND_CONFIG_H
#define FALLING_SAND_CONFIG_H

struct Config {
  const char *title = "Falling Sand";

  int window_width = 800;
  int window_height = 800;

  int grid_width = 800;
  int grid_height = 800;

  int num_chunks_x = 16;
  int num_chunks_y = 16;
  int chunk_margin = 5;
};

#endif //FALLING_SAND_CONFIG_H