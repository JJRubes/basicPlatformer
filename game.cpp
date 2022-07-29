#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include"game.h"

Game::Game() {
  window = nullptr;
  renderer = nullptr;
}

int Game::init() {
  if(window != nullptr || renderer != nullptr) {
    SDL_Log("Window and renderer not null, likely from calling setup twice\n");
    return 1;
  }

  if(SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Initialisation error: %s\n", SDL_GetError());
    return 1;
  }

  int wrval = SDL_CreateWindowAndRenderer(
      640, 480, SDL_WINDOW_SHOWN,
      &window, &renderer);

  if(wrval) {
    SDL_Log("Window and renderer creation error: %s\n", SDL_GetError());
    return 1;
  }

  if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_Log("SDL Image initialisation error: %s\n", IMG_GetError());
    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    return 1;
  }

  return 0;
}

void Game::run() {
  if(init()) {
    return;
  }

  // not really sure how to handle the return values
  // should I just skip straight to the clean/quit?
  if(setup() == 0) {
    draw();
  }

  clean();

  quit();
}

int Game::setup() {
  return 0;
}

void Game::quit() {
  if(window != nullptr) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  if(renderer != nullptr) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }

  IMG_Quit();
  SDL_Quit();
}

SDL_Texture* Game::loadTexture(std::string path) {
  SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
  if(texture == nullptr) {
    SDL_Log("Image loading error: %s\n", IMG_GetError());
  }

  return texture;
}
