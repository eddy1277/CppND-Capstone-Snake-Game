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

void Renderer::RenderOneSnake(std::shared_ptr<Snake> &&snake_ptr,
                              SDL_Rect &&block,
                              std::tuple<Uint8, Uint8, Uint8> &&rgb_b,
                              std::tuple<Uint8, Uint8, Uint8> &&rgb_h) {
  std::lock_guard<std::mutex> uLock(mtx); // lock resource
  auto [r_b, g_b, b_b] = rgb_b; // RGB of body
  auto [r_h, g_h, b_h] = rgb_h; // RGB of head
  if (snake_ptr->alive || SDL_GetTicks() - snake_ptr->time_dead < 2000) {
    // render body using given color
    SDL_SetRenderDrawColor(sdl_renderer, r_b, g_b, b_b, 0xFF);
    for (Snake_Point const &point : snake_ptr->body) {
      block.x = point.x * block.w;
      block.y = point.y * block.h;
      SDL_RenderFillRect(sdl_renderer, &block);
    }
    if (snake_ptr->alive) {
      // render head using given color
      SDL_SetRenderDrawColor(sdl_renderer, r_h, g_h, b_h, 0xFF);
    } else {
      // render head using red color
      SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
    }
    block.x = static_cast<int>(snake_ptr->head_x) * block.w;
    block.y = static_cast<int>(snake_ptr->head_y) * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  } // render nothing after the snake is dead for more than 2 seconds
}

void Renderer::Render(std::vector<std::shared_ptr<Snake>> &snakes,
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

  // Render snake
  std::vector<std::tuple<Uint8, Uint8, Uint8>> body_colors = {
      std::tuple(0xFF, 0xFF, 0xFF),
      std::tuple(0xFF, 0x7A, 0x7A)}; // white and pink for body

  std::vector<std::tuple<Uint8, Uint8, Uint8>> head_colors = {
      std::tuple(0x00, 0x7A, 0xCC),
      std::tuple(0x00, 0xFF, 0x00)}; // blue and green for head

  std::vector<std::future<void>> futures;
  for (std::size_t i = 0; i < players; ++i) {
    futures.emplace_back(std::async(
        std::launch::async, &Renderer::RenderOneSnake, this, snakes.at(i),
        std::move(block), body_colors.at(i), head_colors.at(i)));
  } // move semantics for rvalue
  std::for_each(futures.begin(), futures.end(),
                [](std::future<void> &ftr) { ftr.wait(); });

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(
    std::vector<std::pair<std::string, int>> &&results, std::vector<bool> &&status,
    int fps) {
  std::string title;
  for (std::size_t i = 0; i < players; ++i) {
    title += (results.at(i).first +
              "'s Score: " + std::to_string(results.at(i).second) +
              " (alive: " + std::to_string(status.at(i)) + ") ");
  }
  title += ("FPS: " + std::to_string(fps));
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
