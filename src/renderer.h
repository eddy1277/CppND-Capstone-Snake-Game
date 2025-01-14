#ifndef RENDERER_H
#define RENDERER_H

#include "SDL.h"
#include "snake.h"
#include <algorithm>
#include <future>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

class Renderer {
public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height,
           const std::size_t players);
  ~Renderer();

  void RenderOneSnake(std::shared_ptr<Snake> &&snake_ptr, SDL_Rect &&block,
                      std::tuple<Uint8, Uint8, Uint8> &&rgb_b,
                      std::tuple<Uint8, Uint8, Uint8> &&rgb_h);
  void Render(std::vector<std::shared_ptr<Snake>> &snakes,
              Snake_Point const &food);
  void UpdateWindowTitle(std::vector<std::pair<std::string, int>> &&results,
                         std::vector<bool> &&status, int fps);

private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
  const std::size_t players;
  std::mutex mtx;
};

#endif