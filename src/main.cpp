#include "assert.h"
#include "controller.h"
#include "game.h"
#include "renderer.h"
#include <iostream>
#include <tuple>

int main() {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  std::size_t kPlayers;
  std::string line;
  std::vector<std::string> names;
  std::cout << "Please enter the game mode: 1 for single player mode, 2 for "
               "double player mode:\n";
  while (true) {
    std::getline(std::cin, line);
    try {
      kPlayers = std::stoi(line);
      if (kPlayers != 1 && kPlayers != 2) {
        throw std::invalid_argument(
            "Only 1 player or 2 players are supported. Please reenter.\n");
      }
      break;
    } catch (std::exception &e) {
      std::cout << e.what() << ". Please reenter.\n";
    }
  }
  std::vector<std::tuple<std::string, std::string, std::string, std::string,
                         std::string>>
      config = {std::tuple("blue", "left", "right", "up", "down"),
                std::tuple("green", "z", "c", "s", "x")};
  for (std::size_t i = 0; i < kPlayers; ++i) {
    auto [c, l, r, u, d] = config.at(i);
    std::cout << "The head of snake " << (i + 1) << " has color " << c << ".\n";
    std::cout << "The Left, Right, Up, Down moves are controlled by keys " << l
              << ", " << r << ", " << u << ", " << d << ".\n";
    std::cout << "Please enter the name of player " << (i + 1) << ":\n";
    std::getline(std::cin, line);
    names.push_back(line);
  }

  Game game(kGridWidth, kGridHeight, kPlayers, names);
  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight,
                    kPlayers);
  Controller controller(kPlayers);
  game.Run(controller, renderer, kMsPerFrame);
  std::cout << "Game has terminated successfully!\n";
  for (auto &x : game.GetResults()) {
    std::cout << x.first << "'s Score: " << x.second << "\n";
  }
  game.UpdateRecords();
  return 0;
}