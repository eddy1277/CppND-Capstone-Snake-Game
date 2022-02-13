#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "game.h"
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height, std::string player)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      player(player) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(player, score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  Snake_Point point;
  while (true) {
    point.x = random_w(engine);
    point.y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(point)) {
      food = point;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed = 0.16;//+= 0.02;
  }
}

void Game::UpdateRecords() {
  // initialize records_map
  std::unordered_map<std::string, int> records_map;
  records_map.insert({GetPlayer(), GetScore()});

  std::string line, key, value;
  int ivalue;
  std::fstream filestream;
  // open the record file if exists, otherwise create an empty file
  filestream.open("records.txt", std::fstream::in);
  // read line by line to generate the vector records 
  while (std::getline(filestream, line)) {
    std::replace(line.begin(), line.end(), ' ', '_');
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    linestream >> key >> value;
    std::replace(key.begin(), key.end(), '_', ' ');
    ivalue = std::stoi(value);
    if (records_map.find(key) != records_map.end()) {
      records_map[key] = (records_map[key]>ivalue ? records_map[key] : ivalue);
    }
    else {
      records_map.insert({key, ivalue});
    }
  }
  filestream.close();
  std::vector<std::pair<std::string, int>> records_vector(records_map.begin(), records_map.end());
  std::sort(records_vector.begin(), records_vector.end(), [](auto &left, auto &right) { return left.second > right.second; });
  
  // write the vector records into the file line by line
  filestream.open("records.txt", std::fstream::out | std::fstream::trunc);
  for (auto &x : records_vector) {
    filestream << x.first << ":" << x.second << "\n";
  }
  filestream.close();
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
std::string Game::GetPlayer() const { return player; }