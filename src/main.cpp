#include <iostream>

#include "falling-sand/app.h"

int main() {
  App app;

  if (!app.init()) {
    std::cerr << "Failed to initialize app" << std::endl;
    return -1;
  }

  app.run();
  app.cleanup();

  return 0;
}