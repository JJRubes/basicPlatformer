#include<SDL.h>
#include<fstream>
#include"game.h"
#include"platformer.h"

Platformer::Platformer() {
  spriteSheet = nullptr;
  tiles = nullptr;
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

  readLevel("level1.txt");
  // set colour to magenta to make it obvious
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  return 0;
}

int Platformer::draw() {
  bool quit = false;
  int sprite = 0;
  int steps = 1;

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
          case SDLK_LEFT:
            playerPosX -= 0.5;
            break;
          case SDLK_RIGHT:
            playerPosX += 0.5;
            break;
          case SDLK_UP:
            playerPosY -= 0.5;
            break;
          case SDLK_DOWN:
            playerPosY += 0.5;
            break;
          default:
            break;
        }
      }
    }

    moveScreenToPosition(playerPosX, playerPosY, steps);
    if(steps > 1)
      steps--;
    renderScreen();
  }

  return 0;
}

// draws the background tiles then the player on top
void Platformer::renderScreen() {
  // clear the screen
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  SDL_RenderClear(renderer);

  // get the x and y offsets from the top left of the nearest tile
  int tileOffsetX = screenX % int(SCALED_SPRITE_WIDTH);
  if(screenX < 0) {
    tileOffsetX += SCALED_SPRITE_WIDTH;
  }
  int tileOffsetY = screenY % int(SCALED_SPRITE_WIDTH);
  if(screenY < 0) {
    tileOffsetY += SCALED_SPRITE_WIDTH;
  }

  // add one so that during scrolling there isn't a gap
  for(int i = 0; i < TILED_SCREEN_HEIGHT + 1; i++) {
    for(int j = 0; j < TILED_SCREEN_WIDTH + 1; j++) {
      int screenTilePosX = SCALED_SPRITE_WIDTH * j - tileOffsetX;
      int screenTilePosY = SCALED_SPRITE_WIDTH * i - tileOffsetY;
      if(screenTileX + j < 0 || screenTileX + j >= levelW
          || screenTileY + i < 0 || screenTileY + i >= levelH) {
        drawSprite(0, screenTilePosX, screenTilePosY, SPRITE_SCALE);
      } else {
        drawSprite(tiles[screenTileY + i][screenTileX + j], screenTilePosX, screenTilePosY, SPRITE_SCALE);
      }
    }
  }

  SDL_RenderPresent(renderer);
}

void Platformer::tileFromScreen() {
  screenTileX = screenX / SCALED_SPRITE_WIDTH;
  if(screenX < 0) {
    screenTileX--;
  }
  screenTileY = screenY / SCALED_SPRITE_WIDTH;
  if(screenY < 0) {
    screenTileY--;
  }
}

// move the screen so that it is centred on a given tile
void Platformer::moveScreenToPosition(float x, float y, int step) {
  // find the top left of the player
  int playerWorldPosX = int(x * SCALED_SPRITE_WIDTH);
  int playerWorldPosY = int(y * SCALED_SPRITE_WIDTH);

  // subtract half the screen width to centre the top-left
  // then add half of the sprite width to centre on the middle of the tile
  int centredX = playerWorldPosX - 640 / 2 + SCALED_SPRITE_WIDTH / 2;
  int centredY = playerWorldPosY - 480 / 2 + SCALED_SPRITE_WIDTH / 2;
  screenX = screenX + (1.0 / step) * (centredX - screenX);
  screenY = screenY + (1.0 / step) * (centredY - screenY);
}

int Platformer::clean() {
  if(spriteSheet != nullptr) {
    SDL_DestroyTexture(spriteSheet);
  }
  return 0;
}

// currently opens a file and reads whitespace seperated integers
// TODO checks for tile numbers within bounds
// TODO how to handle if there is already a level loaded
void Platformer::readLevel(std::string filename) {
  // check that tiles has no data
  if(tiles != nullptr) {
    SDL_Log("Attempted to read new level data while level data already loaded\n");
    return;
  }

  // load filename in text mode
  std::ifstream levelData(filename);
  // make sure that the file opened
  if(levelData.is_open()) {
    levelData >> levelW >> levelH;
    tiles = new int*[levelH];
    for(int i = 0; i < levelH; i++) {
      tiles[i] = new int[levelW];
      for(int j = 0; j < levelW; j++) {
        levelData >> tiles[i][j];
      }
    }
  } else {
    SDL_Log("Error loading level data file\n");
    return;
  }
}

void Platformer::saveLevel(std::string filename) {
  if(tiles == nullptr) {
    SDL_Log("Attempted save without any level data\n");
    return;
  }

  std::ofstream levelData(filename);
  if(levelData.is_open()) {
    levelData << levelW << " " << levelH << " ";
    for(int i = 0; i < levelH; i++) {
      for(int j = 0; j < levelW; j++) {
        levelData << tiles[i][j] << " ";
      }
    }
  } else {
    SDL_Log("Failed to open file to write level data to\n");
    return;
  }
}
