#include <iostream>

#include "falling-sand/app.h"

int main() {
  // create and run the app
  App app(Config{});

  if (!app.init()) {
    std::cerr << "Failed to initialize app" << std::endl;
    return -1;
  }

  app.run();

  return 0;
}