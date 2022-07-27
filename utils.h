#include<SLD.h>
#include<SDL_image.h>
#include<string>

SDL_Window* init() {
  SDL_Window* window = nullptr;
  if(SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Initialisation error: %s", SDL_GetError());
    return nullptr;
  }

  window = SDL_CreateWindow("Test",
		  SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      640, 480, SDL_WINDOW_SHOWN);

  if(window == nullptr) {
    SDL_Log("Window creation error: %s", SDL_GetError());
    return nullptr;
  }

  if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_Log("SDL Image initialisation error: %s", IMG_GetError());
    SDL_DestroyWindow(window);
    return nullptr;
  }

  return window;
}

SDL_Surface* loadSurface(std::string path) {
  SDL_Surface* optimisedSurface = nullptr;

  SDL_Surface* rawSurface = IMG_Load(path.c_str());
  if(rawSurface == nullptr) {
    SDL_Log("Image loading error: %s", IMG_GetError());
  } else {
    optimisedSurface = SDL_ConvertSurface(rawSurface, format, 0);
    if(optimisedSurface == nullptr) {
      SLD_Log("Image optimisation error: %s", SDL_GetError());
    }

    SDL_FreeSurface(rawSurface);
  }

  return optimisedSurface;
}
