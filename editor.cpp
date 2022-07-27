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
          default:
            break;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
    SDL_RenderClear(renderer);
    int scale = 4;
    int scaledW = scale * SPRITE_WIDTH;
    int tilesInWidth =  640 / (2 * scaledW);
    int tilesInHeight = 480 / (2 * scaledW);
    int tilePosX = worldPosX / scaledW;
    int tilePosY = worldPosY / scaledW;
    int tileOffsetX = worldPosX % scaledW;
    if(tileOffsetX < 0) {
      tileOffsetX += scaledW;
    }
    int tileOffsetY = worldPosY % scaledW;
    if(tileOffsetY < 0) {
      tileOffsetY += scaledW;
    }
    for(int i = 0; i < tilesInHeight + 1; i++) {
      for(int j = 0; j < tilesInWidth + 1; j++) {
        /*** Something is seriously jank here, have a look tomorrow ***/

        // world (0, 0) is the same point as tile (0, 0)
        // screen is the offset from (0, 0) of the top left of the screen
        // find out where it is in a tile
        // subtract that from the i and j
        int screenTilePosX = scaledW * j - tileOffsetX + 160;
        int screenTilePosY = scaledW * i - tileOffsetY + 120;
        if(tilePosX + j < 0 || tilePosX + j > 32
            || tilePosY + i < 0 || tilePosY + i > 16) {
          // quit = true;
          // SDL_Log("Drawing backup tile at (%d, %d), where tile pos is (%d, %d), and world pos is (%d, %d)\n", x, y, tilePosX, tilePosY, worldPosX, worldPosY);
          drawSprite(0, screenTilePosX, screenTilePosY, scale);
        } else {
          drawSprite(tiles[tilePosY + i][tilePosX + j], screenTilePosX, screenTilePosY, scale);
        }
      }
    }
    SDL_Rect rect = {160, 120, 320, 240};
    SDL_SetRenderDrawColor(renderer, 0x00, 0xaa, 0x2f, 0xff);
    SDL_RenderDrawRect(renderer, &rect);
    int half = scale * SPRITE_WIDTH / 2;
    drawSprite(brush, half, 480 - 3 * half, scale);
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
