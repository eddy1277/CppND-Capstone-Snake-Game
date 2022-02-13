#ifndef RENDERER_H
#define RENDERER_H

#include "SDL.h"
#include "snake.h"
#include <memory>
#include <string>
#include <vector>

class Renderer {
public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Render(std::shared_ptr<Snake> snake, Snake_Point const &food);
  void UpdateWindowTitle(std::string name, int score, int fps);

private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
};

#endif