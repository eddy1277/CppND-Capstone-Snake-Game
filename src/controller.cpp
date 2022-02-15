#include "controller.h"
#include "SDL.h"
#include "snake.h"
#include <iostream>

void Controller::ChangeDirection(std::shared_ptr<Snake> snake,
                                 Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake->direction != opposite || snake->size == 1)
    snake->direction = input;
  return;
}

void Controller::HandleInput(bool &running,
                             std::vector<std::shared_ptr<Snake>> snakes) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {

      if (players == 1) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
          ChangeDirection(snakes.at(0), Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
          ChangeDirection(snakes.at(0), Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
          ChangeDirection(snakes.at(0), Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
          ChangeDirection(snakes.at(0), Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;
        }
      } else {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
          ChangeDirection(snakes.at(0), Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
          ChangeDirection(snakes.at(0), Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
          ChangeDirection(snakes.at(0), Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
          ChangeDirection(snakes.at(0), Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;

        case SDLK_s:
          ChangeDirection(snakes.at(1), Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_x:
          ChangeDirection(snakes.at(1), Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_z:
          ChangeDirection(snakes.at(1), Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_c:
          ChangeDirection(snakes.at(1), Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;
        }
      }
    }
  }
}