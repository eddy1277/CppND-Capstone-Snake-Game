#include "renderer.h"
#include <iostream>
#include <tuple>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height,
                   const std::size_t players)
    : screen_width(screen_width), screen_height(screen_height),
      grid_width(grid_width), grid_height(grid_height), players(players) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::Render(std::vector<std::shared_ptr<Snake>> snakes,
                      Snake_Point const &food) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake's body
  std::vector<std::tuple<SDL_Renderer *, Uint8, Uint8, Uint8, Uint8>>
      body_colors = {
          std::tuple(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF),
          std::tuple(sdl_renderer, 0xFF, 0x7A, 0x7A, 0xFF)}; // white and pink
  for (std::size_t i = 0; i < players; ++i) {
    std::apply(SDL_SetRenderDrawColor, body_colors.at(i));
    for (Snake_Point const &point : snakes.at(i)->body) {
      block.x = point.x * block.w;
      block.y = point.y * block.h;
      SDL_RenderFillRect(sdl_renderer, &block);
    }
  }

  // Render snake's head
  std::vector<std::tuple<SDL_Renderer *, Uint8, Uint8, Uint8, Uint8>>
      head_colors = {
          std::tuple(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF),
          std::tuple(sdl_renderer, 0x00, 0xFF, 0x00, 0xFF)}; // blue and green
  for (std::size_t i = 0; i < players; ++i) {
    block.x = static_cast<int>(snakes.at(i)->head_x) * block.w;
    block.y = static_cast<int>(snakes.at(i)->head_y) * block.h;
    if (snakes.at(i)->alive) {
      std::apply(SDL_SetRenderDrawColor, head_colors.at(i));
    } else {
      SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
    }
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(
    std::vector<std::pair<std::string, int>> results, int fps) {
  std::string title;
  for (auto &result : results) {
    title +=
        (result.first + "'s Score: " + std::to_string(result.second) + " ");
  }
  title += ("FPS: " + std::to_string(fps));
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
