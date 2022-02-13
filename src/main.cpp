#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include "controller.h"
#include "game.h"
#include "renderer.h"

void UpdateRecords(std::string player, int score) {
  std::vector<std::pair<std::string, int>> records;
  std::string line, key, value;
  int ivalue;
  bool new_player = true;
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
    if (player == key) {
      ivalue = (ivalue > score ? ivalue : score);
      new_player = false;
    }
    records.push_back({key, ivalue});
  }
  if (new_player) {
    records.push_back({player, score});
  }
  std::sort(records.begin(), records.end(),
            [](auto &left, auto &right) { return left.second > right.second; });
  filestream.close();

  // write the vector records into the file line by line
  filestream.open("records.txt", std::fstream::out | std::fstream::trunc);
  for (auto &x : records) {
    filestream << x.first << ":" << x.second << "\n";
  }
  filestream.close();
}

int main() {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  Controller controller;
  std::string player;
  std::cout << "Please enter the player name:\n";
  std::getline(std::cin, player);
  Game game(kGridWidth, kGridHeight, player);
  game.Run(controller, renderer, kMsPerFrame);
  std::cout << "Game has terminated successfully!\n";
  std::cout << "Score: " << game.GetScore() << "\n";
  std::cout << "Size: " << game.GetSize() << "\n";
  UpdateRecords(game.GetPlayer(), game.GetScore());
  return 0;
}