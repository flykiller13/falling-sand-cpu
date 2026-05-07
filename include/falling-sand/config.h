#ifndef FALLING_SAND_CONFIG_H
#define FALLING_SAND_CONFIG_H

// used to initialize the app with default values
struct Config {
  const char *title = "Falling Sand";

  int window_width = 800;
  int window_height = 800;

  // grid size can be different from window size - cells will be stretched
  int grid_width = 400;
  int grid_height = 400;

  // chunk info - 16x16 is a good compromise. change based on grid size.
  // doesn't have to be aligned to grid/window size.
  int num_chunks_x = 16;
  int num_chunks_y = 16;
  int chunk_margin = 5; // overlap margin for waking up neighboring chunks
};

#endif //FALLING_SAND_CONFIG_H