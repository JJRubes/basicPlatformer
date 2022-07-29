#include<SDL.h>
#include"game.h"
#include"platformer.h"

Platformer::Platformer() {
  spriteSheet = nullptr;
}

void Platformer::drawSprite(int sprite, int x, int y, int scale) {
  SDL_Rect clip = {(sprite % SHEET_WIDTH) * SPRITE_WIDTH,
    (sprite / SHEET_WIDTH) * SPRITE_WIDTH,
    SPRITE_WIDTH, SPRITE_WIDTH};
  SDL_Rect position = {x, y, scale * SPRITE_WIDTH, scale * SPRITE_WIDTH};
  SDL_RenderCopy(renderer, spriteSheet, &clip, &position);
}

int Platformer::setup() {
  spriteSheet = loadTexture("troid_spritesheetsmall.png");
  if(spriteSheet == nullptr) {
    SDL_Log("spriteSheet failed to load\n");
    return 1;
  }

  // set colour to magenta to make it obvious
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  return 0;
}

int Platformer::draw() {
  bool quit = false;
  int sprite = 0;

  while(!quit) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT) {
        quit = true;
      } else if(e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
          case 'q':
            quit = true;
            break;
          case 'n':
            sprite++;
            sprite %= (SHEET_WIDTH * SHEET_HEIGHT);
            break;
          default:
            break;
        }
      }
    }

    SDL_RenderClear(renderer);
    int scale = 4;
    for(int i = 0; i < 480; i += scale * SPRITE_WIDTH) {
      for(int j = 0; j < 640; j += scale * SPRITE_WIDTH) {
        drawSprite(sprite, j, i, scale);
      }
    }
    SDL_RenderPresent(renderer);
  }

  return 0;
}


int Platformer::clean() {
  if(spriteSheet != nullptr) {
    SDL_DestroyTexture(spriteSheet);
  }
  return 0;
}
