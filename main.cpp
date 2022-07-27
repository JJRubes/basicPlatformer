// tileset from
// https://opengameart.org/content/16x16-dark-tech-base-tileset
// because I didn't have the want to make my own

#include<SDL.h>
#include<SDL_image.h>
#include<string>

int main() {
  SDL_Window* window = init();
  if(window == nullptr) {
    return 1;
  }

  bool quit = false;

  while(!quit) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT) {
        quit = true;
      }
    }
  }

  SDL_DestroyWindow(window);
  window = nullptr;

  IMG_Quit();
  SDL_Quit();
  return 0;
}
