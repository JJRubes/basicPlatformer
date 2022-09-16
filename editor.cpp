#include<SDL.h>
#include<fstream>
#include"game.h"
#include"editor.h"

Editor::Editor() {
  spriteSheet = nullptr;
  screenPosX = 0;
  screenPosY = 0;
  tileX = 0;
  tileY = 0;
  screenTileX = 0;
  screenTileY = 0;
  tiles = nullptr;
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

  readLevel("level1.txt");

  // set colour to magenta to make it obvious
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  return 0;
}

int Editor::draw() {
  bool quit = false;
  int brush = 0;
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
          case 'n':
            if(brush == 64)
              brush = 3;
            else
              brush = 64;
            // brush++;
            // brush %= (SHEET_WIDTH * SHEET_HEIGHT);
            break;
          case 'h':
            if(tileX > 0) {
              tileX--;
              steps = 100;
            }
            break;
          case 'l':
            if(tileX < levelW - 1) {
              tileX++;
              steps = 100;
            }
            break;
          case 'k':
            if(tileY > 0) {
              tileY--;
              steps = 100;
            }
            break;
          case 'j':
            if(tileY < levelH - 1) {
              tileY++;
              steps = 100;
            }
            break;
          case 'a':
            SDL_Log("screenPos: (%d, %d), tile: (%d, %d)\n", screenPosX, screenPosY, tileX, tileY);
            break;
          case '-':
            setScale(getScale() - 1);
            break;
          case '=':
            setScale(getScale() + 1);
            break;
          case 's':
            saveLevel("levelOut.level");
            break;
          case SDLK_RETURN:
            tiles[tileY][tileX] = brush;
            break;
          default:
            break;
        }
      }
    }

    moveScreenToTile(tileX, tileY, steps);
    if(steps > 1) {
      steps--;
    }
    renderScreen(brush);
  }

  return 0;
}


int Editor::clean() {
  if(spriteSheet != nullptr) {
    SDL_DestroyTexture(spriteSheet);
  }

  return 0;
}

// once all variables are set call to render the screen
void Editor::renderScreen(int brush) {
  // clear the screen
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  SDL_RenderClear(renderer);

  tileFromScreen();
  int tileOffsetX = screenPosX % SCALED_SPRITE_WIDTH;
  if(screenPosX < 0) {
    tileOffsetX += SCALED_SPRITE_WIDTH;
  }
  int tileOffsetY = screenPosY % SCALED_SPRITE_WIDTH;
  if(screenPosY < 0) {
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
  int half = SPRITE_SCALE * SPRITE_WIDTH / 2;
  SDL_Rect rect = {320 - half, 240 - half, SCALED_SPRITE_WIDTH, SCALED_SPRITE_WIDTH};
  SDL_SetRenderDrawColor(renderer, 0x00, 0xaa, 0x2f, 0xff);
  SDL_RenderDrawRect(renderer, &rect);
  drawSprite(brush, half, 480 - 3 * half, SPRITE_SCALE);
  SDL_RenderPresent(renderer);
}

void Editor::tileFromScreen() {
  screenTileX = screenPosX / SCALED_SPRITE_WIDTH;
  if(screenPosX < 0) {
    screenTileX--;
  }
  screenTileY = screenPosY / SCALED_SPRITE_WIDTH;
  if(screenPosY < 0) {
    screenTileY--;
  }
}

// move the screen so that it is centred on a given tile
void Editor::moveScreenToTile(int tx, int ty, int step) {
  // find the top left of the tile
  int tWorldPosX = tx * SCALED_SPRITE_WIDTH;
  int tWorldPosY = ty * SCALED_SPRITE_WIDTH;

  // subtract half the screen width to centre the top-left
  // then add half of the sprite width to centre on the middle of the tile
  int centredX = tWorldPosX - 640 / 2 + SCALED_SPRITE_WIDTH / 2;
  int centredY = tWorldPosY - 480 / 2 + SCALED_SPRITE_WIDTH / 2;
  screenPosX = screenPosX + (1.0 / step) * (centredX - screenPosX);
  screenPosY = screenPosY + (1.0 / step) * (centredY - screenPosY);
}

void Editor::setScale(int scale) {
  SPRITE_SCALE = scale;
  SCALED_SPRITE_WIDTH = SPRITE_SCALE * SPRITE_WIDTH;
  TILED_SCREEN_WIDTH = (640 / SCALED_SPRITE_WIDTH) + 1;
  TILED_SCREEN_HEIGHT = (480 / SCALED_SPRITE_WIDTH) + 1;
}

int Editor::getScale() {
  return SPRITE_SCALE;
}

// currently opens a file and reads whitespace seperated integers
// TODO checks for tile numbers within bounds
// TODO how to handle if there is already a level loaded
void Editor::readLevel(std::string filename) {
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
        if(tiles[i][j] <= 0 || tiles[i][j] >= 80)
          tiles[i][j] = 64;
      }
    }
  } else {
    SDL_Log("Error loading level data file\n");
    return;
  }
}

void Editor::saveLevel(std::string filename) {
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
