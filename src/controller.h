#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include <memory>

class Controller {
 public:
  void HandleInput(bool &running, std::vector<std::shared_ptr<Snake>> snakes) const;
  Controller(std::size_t players): players(players) {}

 private:
  void ChangeDirection(std::shared_ptr<Snake> snake, Snake::Direction input,
                       Snake::Direction opposite) const;
  std::size_t players;
};

#endif