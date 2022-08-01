#include<SDL.h>
#include"game.h"
#include"editor.h"

Editor::Editor() {
  spriteSheet = nullptr;
  screenPosX = 0;
  screenPosY = 0;
  tileX = 0;
  tileY = 0;
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

  // screenPosX = -(640 - SCALED_SPRITE_WIDTH) / 2;
  // screenPosY = -(480 - SCALED_SPRITE_WIDTH) / 2;
  tileX = 0;
  tileY = 0;


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
            tileX--;
            break;
          case 'l':
            tileX++;
            break;
          case 'k':
            tileY--;
            break;
          case 'j':
            tileY++;
            break;
          case 'a':
            SDL_Log("screenPos: (%d, %d), tile: (%d, %d)\n", screenPosX, screenPosY, tileX, tileY);
          default:
            break;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
    SDL_RenderClear(renderer);
    moveScreenToTile(tileX, tileY);
    // tileFromScreen();
    int tileOffsetX = 0;
    // int tileOffsetX = screenPosX % SCALED_SPRITE_WIDTH;
    // if(screenPosX < 0) {
    //   tileOffsetX += SCALED_SPRITE_WIDTH;
    // }
    int tileOffsetY = 0;
    // int tileOffsetY = screenPosY % SCALED_SPRITE_WIDTH;
    // if(screenPosY < 0) {
    //   tileOffsetY += SCALED_SPRITE_WIDTH;
    // }

    // add one so that during scrolling there isn't a gap
    for(int i = 0; i < TILED_SCREEN_HEIGHT + 1; i++) {
      for(int j = 0; j < TILED_SCREEN_WIDTH + 1; j++) {
        int screenTilePosX = SCALED_SPRITE_WIDTH * j - tileOffsetX;
        int screenTilePosY = SCALED_SPRITE_WIDTH * i - tileOffsetY;
        if(tileX + j < 0 || tileX + j >= 16
            || tileY + i < 0 || tileY + i >= 5) {
          drawSprite(0, screenTilePosX, screenTilePosY, SPRITE_SCALE);
        } else {
          drawSprite(tiles[tileY + i][tileX + j], screenTilePosX, screenTilePosY, SPRITE_SCALE);
        }
      }
    }
    int half = SPRITE_SCALE * SPRITE_WIDTH / 2;
    SDL_Rect rect = {320 - half, 240 - half, SCALED_SPRITE_WIDTH, SCALED_SPRITE_WIDTH};
    SDL_SetRenderDrawColor(renderer, 0x00, 0xaa, 0x2f, 0xff);
    SDL_RenderDrawRect(renderer, &rect);
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

void Editor::tileFromScreen() {
  tileX = screenPosX / SCALED_SPRITE_WIDTH;
  if(screenPosX < 0) {
    tileX--;
  }
  tileY = screenPosY / SCALED_SPRITE_WIDTH;
  if(screenPosY < 0) {
    tileY--;
  }
}

// move the screen so that it is centred on a given tile
void Editor::moveScreenToTile(int tx, int ty) {
  // find the top left of the tile
  int tWorldPosX = tx * SCALED_SPRITE_WIDTH;
  int tWorldPosY = ty * SCALED_SPRITE_WIDTH;

  // subtract half the screen width to centre the top-left
  // then add half of the sprite width to centre on the middle of the tile
  screenPosX = tWorldPosX - 640 / 2 + SCALED_SPRITE_WIDTH / 2;
  screenPosY = tWorldPosY - 480 / 2 + SCALED_SPRITE_WIDTH / 2;
}
