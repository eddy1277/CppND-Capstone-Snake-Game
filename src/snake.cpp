#include "snake.h"
#include <cmath>
#include <iostream>

Snake::Snake(int grid_width, int grid_height, float head_x, float head_y,
             std::string name)
    : grid_width(grid_width), grid_height(grid_height), head_x(head_x),
      head_y(head_y), name(name) {}

Snake::Snake(const Snake &source) {
  grid_width = source.grid_width;
  grid_height = source.grid_height;
  head_x = source.head_x;
  head_y = source.head_y;
  name = source.name;
}

Snake::Snake(Snake &&source) {
  grid_width = source.grid_width;
  grid_height = source.grid_height;
  head_x = source.head_x;
  head_y = source.head_y;
  name = source.name;
}

Snake Snake::operator=(Snake &&source) {
  grid_width = source.grid_width;
  grid_height = source.grid_height;
  head_x = source.head_x;
  head_y = source.head_y;
  name = source.name;
  return *this;
}

void Snake::Update() {
  Snake_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)}; // We first capture the head's cell before updating.
  UpdateHead();
  Snake_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)}; // Capture the head's cell after updating.
  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }
}

void Snake::UpdateHead() {
  switch (direction) {
  case Direction::kUp:
    head_y -= speed;
    break;

  case Direction::kDown:
    head_y += speed;
    break;

  case Direction::kLeft:
    head_x -= speed;
    break;

  case Direction::kRight:
    head_x += speed;
    break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(Snake_Point &current_head_cell,
                       Snake_Point &prev_head_cell) {
  // Add previous head location to vector and unordered_set
  body.push_back(prev_head_cell);
  body_set.insert(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector and unordered_set
    body_set.erase(*body.begin());
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }

  // Check if the snake has died using hash table
  if (body_set.find(current_head_cell) != body_set.end()) {
    alive = false;
  }
}

// Check if cell is occupied by snake using hash table
bool Snake::SnakeCell(const Snake_Point &point) const {
  if (point.x == static_cast<int>(head_x) &&
      point.y == static_cast<int>(head_y)) {
    return true;
  }
  if (body_set.find(point) != body_set.end()) {
    return true;
  }
  return false;
}