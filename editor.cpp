#include<SDL.h>
#include"game.h"
#include"editor.h"

Editor::Editor() {
  spriteSheet = nullptr;
}

void Editor::drawSprite(int sprite, int x, int y, int scale) {
  SDL_Rect clip = {(sprite % SHEET_WIDTH) * SPRITE_WIDTH,
    (sprite / SHEET_WIDTH) * SPRITE_WIDTH,
    SPRITE_WIDTH, SPRITE_WIDTH};
  SDL_Rect position = {x, y, scale * SPRITE_WIDTH, scale * SPRITE_WIDTH};
  SDL_RenderCopy(renderer, spriteSheet, &clip, &position);
}

int Editor::setup() {
  spriteSheet = loadTexture("troid_spritesheetsmall.png");
  if(spriteSheet == nullptr) {
    SDL_Log("spriteSheet failed to load\n");
    return 1;
  }

  SDL_Log("tiles per width: %d, tiles per height %d\n",
      TILED_SCREEN_WIDTH,
      TILED_SCREEN_HEIGHT);

  // set colour to magenta to make it obvious
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  return 0;
}

int Editor::draw() {
  bool quit = false;
  int brush = 0;

  int worldPosX = 0;
  int worldPosY = 0;

  int tiles[5][16];
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 16; j++) {
      tiles[i][j] = 16 * i + j;
    }
  }

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
            brush++;
            brush %= (SHEET_WIDTH * SHEET_HEIGHT);
            break;
          case 'h':
            worldPosX--;
            break;
          case 'l':
            worldPosX++;
            break;
          case 'k':
            worldPosY--;
            break;
          case 'j':
            worldPosY++;
            break;
          case 'a':
            SDL_Log("(%d, %d)\n", worldPosX, worldPosY);
          default:
            break;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
    SDL_RenderClear(renderer);
    int tileX = worldPosX / SCALED_SPRITE_WIDTH;
    if(worldPosX < 0) {
      tileX--;
    }
    int tileY = worldPosY / SCALED_SPRITE_WIDTH;
    if(worldPosY < 0) {
      tileY--;
    }
    int tileOffsetX = worldPosX % SCALED_SPRITE_WIDTH;
    if(worldPosX < 0) {
      tileOffsetX += SCALED_SPRITE_WIDTH;
    }
    int tileOffsetY = worldPosY % SCALED_SPRITE_WIDTH;
    if(worldPosY < 0) {
      tileOffsetY += SCALED_SPRITE_WIDTH;
    }
    // Why do I need to add 2 here when it should be 1?
    // is TILED_SCREEN_HEIGHT incorrect?
    for(int i = 0; i < TILED_SCREEN_HEIGHT + 2; i++) {
      for(int j = 0; j < TILED_SCREEN_WIDTH + 1; j++) {
        int screenTilePosX = SCALED_SPRITE_WIDTH * j - tileOffsetX + 160;
        int screenTilePosY = SCALED_SPRITE_WIDTH * i - tileOffsetY + 120;
        if(tileX + j < 0 || tileX + j >= 16
            || tileY + i < 0 || tileY + i >= 5) {
          drawSprite(0, screenTilePosX, screenTilePosY, SPRITE_SCALE);
        } else {
          drawSprite(tiles[tileY + i][tileX + j], screenTilePosX, screenTilePosY, SPRITE_SCALE);
        }
      }
    }
    SDL_Rect rect = {160, 120, 320, 240};
    SDL_SetRenderDrawColor(renderer, 0x00, 0xaa, 0x2f, 0xff);
    SDL_RenderDrawRect(renderer, &rect);
    int half = SPRITE_SCALE * SPRITE_WIDTH / 2;
    drawSprite(brush, half, 480 - 3 * half, SPRITE_SCALE);
    SDL_RenderPresent(renderer);
  }

  return 0;
}


int Editor::clean() {
  if(spriteSheet != nullptr) {
    SDL_DestroyTexture(spriteSheet);
  }
  return 0;
}
