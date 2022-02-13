#ifndef SNAKE_H
#define SNAKE_H

#include "SDL.h"
#include <string>
#include <unordered_set>
#include <vector>

struct Snake_Point: SDL_Point {
    // overload copy assignment operator
    Snake_Point operator=(const Snake_Point &other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    // overload == operator
    bool operator==(const Snake_Point &other) const {
        return (other.x == x) && (other.y == y);
    }
};

// hash function for Snake_Point
struct Snake_Hash {
    std::size_t operator() (const Snake_Point &point) const {
        return std::hash<int>()(point.x) ^ std::hash<int>()(point.y);
    }
};

class Snake {
public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height, std::string name)
      : grid_width(grid_width), 
        grid_height(grid_height),
        head_x(grid_width / 2), 
        head_y(grid_height / 2),
        name(name) {}

  void Update();
  bool SnakeCell(const Snake_Point &point) const;

  // setters and getters
  void GrowBody() { growing = true; }
  void AddScore() { score++; }
  int GetSnakeScore() const { return score; };
  std::string GetSnakeName() const { return name; };

  Direction direction = Direction::kUp;
  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<Snake_Point> body;
  std::unordered_set<Snake_Point, Snake_Hash> body_set;

private:
  void UpdateHead();
  void UpdateBody(Snake_Point &current_cell, Snake_Point &prev_cell);

  bool growing{false};
  int grid_width;
  int grid_height;
  int score{0};
  std::string name;
};

#endif