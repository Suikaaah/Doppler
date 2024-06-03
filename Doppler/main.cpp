#include <memory>
#include <iostream>

#include "engine.hpp"

int main(int, char**) {
  auto engine = std::make_unique<Engine>();
  engine->run();

  return 0;
}
