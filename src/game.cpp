#include "game.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

Game::Game(std::size_t grid_width, std::size_t grid_height, std::size_t players,
           std::vector<std::string> &names)
    : players(players), engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  Snake_Point point;
  std::unordered_set<Snake_Point, Snake_Hash> heads;
  for (std::size_t i = 0; i < players; ++i) {
    while (true) {
      // select random point for the head of each snake
      point.x = random_w(engine);
      point.y = random_h(engine);
      if (heads.find(point) == heads.end()) {
        snakes.emplace_back(std::make_shared<Snake>(
            grid_width, grid_height, point.x, point.y, names.at(i)));
        heads.insert(point);
        break;
      }
    }
  }
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
    controller.HandleInput(running, snakes);
    Update();
    renderer.Render(snakes, food);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(GetResults(), GetStatus(), frame_count);
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
  bool occupied{false};
  while (true) {
    point.x = random_w(engine);
    point.y = random_h(engine);
    // Check that the location is not occupied by an alive snake before placing
    // food.
    for (std::size_t i = 0; i < players; ++i) {
      if (snakes.at(i)->alive && snakes.at(i)->SnakeCell(point)) {
        occupied = true;
      }
    }
    if (!occupied) {
      food = point;
      return;
    }
  }
}

void Game::UpdateOneSnake(std::shared_ptr<Snake> &&snake_ptr) {
  if (snake_ptr->alive) {
    snake_ptr->Update();
    int new_x = static_cast<int>(snake_ptr->head_x);
    int new_y = static_cast<int>(snake_ptr->head_y);
    // Check if there's food over here
    // use mutex to lock the shared resources (Snake_Point food)
    std::lock_guard<std::mutex> uLock(mtx);
    if (food.x == new_x && food.y == new_y) {
      snake_ptr->AddScore();
      PlaceFood();
      // Grow snake and increase speed.
      snake_ptr->GrowBody();
      snake_ptr->speed += 0.02;
    }
  } // if the snake is dead, do nothing
}

void Game::Update() {
  std::vector<std::future<void>> futures;
  for (std::size_t i = 0; i < players; ++i) {
    futures.emplace_back(std::async(std::launch::async, &Game::UpdateOneSnake,
                                    this, snakes.at(i)));
  }
  std::for_each(futures.begin(), futures.end(),
                [](std::future<void> &ftr) { ftr.wait(); });
}

void Game::UpdateRecords() const {
  // initialize records_map
  std::vector<std::pair<std::string, int>> game_results = GetResults();
  std::unordered_map<std::string, int> records_map(game_results.begin(),
                                                   game_results.end());

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
      records_map[key] = std::max(records_map[key], ivalue);
    } else {
      records_map.insert({key, ivalue});
    }
  }
  filestream.close();
  std::vector<std::pair<std::string, int>> records_vector(records_map.begin(),
                                                          records_map.end());
  std::sort(records_vector.begin(), records_vector.end(),
            [](auto &a, auto &b) { return a.second > b.second; });

  // write the vector records into the file line by line
  filestream.open("records.txt", std::fstream::out | std::fstream::trunc);
  for (auto &a : records_vector) {
    filestream << a.first << ":" << a.second << "\n";
  }
  filestream.close();
}

std::vector<int> Game::GetScores() const {
  std::vector<int> scores;
  for (std::size_t i = 0; i < players; ++i) {
    scores.push_back(snakes.at(i)->GetSnakeScore());
  }
  return scores;
}

std::vector<std::string> Game::GetNames() const {
  std::vector<std::string> names;
  for (std::size_t i = 0; i < players; ++i) {
    names.push_back(snakes.at(i)->GetSnakeName());
  }
  return names;
}

std::vector<bool> Game::GetStatus() const {
  std::vector<bool> status;
  for (std::size_t i = 0; i < players; ++i) {
    status.push_back(snakes.at(i)->alive);
  }
  return status;
}

std::vector<std::pair<std::string, int>> Game::GetResults() const {
  std::vector<std::string> names = GetNames();
  std::vector<int> scores = GetScores();
  std::vector<std::pair<std::string, int>> results;
  for (std::size_t i = 0; i < players; ++i) {
    results.push_back({names.at(i), scores.at(i)});
  }
  return results;
}