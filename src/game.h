#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include <memory>
#include <random>

class Game {
public:
  Game(std::size_t grid_width, std::size_t grid_height, std::size_t players,
       std::vector<std::string> names);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  void UpdateRecords();
  std::vector<int> GetScores() const;
  std::vector<std::string> GetNames() const;
  std::vector<std::pair<std::string, int>> GetResults() const;

private:
  std::vector<std::shared_ptr<Snake>> snakes; // vector of shared pointers
  Snake_Point food;

  std::size_t players;
  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  void PlaceFood();
  void Update();
};

#endif