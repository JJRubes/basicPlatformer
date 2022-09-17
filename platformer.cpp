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

  xVel = 0;
  yVel = 0;

  // set colour to magenta to make it obvious
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  return 0;
}

void Platformer::physics(double deltaTime,
    bool jumping, bool lefting, bool righting) {
  float acc = 0.000015;
  float jump = 0.01;
  float gravity = 0.00003;
  float maxXVel = 0.006;

  float playerWidth = 0.5;
  bool onGround = tiles[int(playerPosY) + 1][int(playerPosX + playerWidth / 2)] == 3 ||
      tiles[int(playerPosY) + 1][int(playerPosX + 1 - playerWidth / 2)] == 3;

  // if(onGround)
  //   playerPosY = floor(playerPosY);

  if(jumping && onGround && yVel == 0)
    yVel -= jump;

  if(lefting) {
    if(onGround)
      xVel -= acc * deltaTime;
    else
      xVel -= 0.75 * acc; // less control in the air
  }
  
  if(righting) {
    if(onGround)
      xVel += acc * deltaTime;
    else
      xVel += 0.75 * acc * deltaTime;
  }

  // not sure how to incorporate friction with delta time
  if(onGround && !lefting && !righting)
    xVel *= 0.995; // friction

  // the falling slowdown is a bit less
  if(!onGround && !lefting && !righting)
    xVel *= 0.999; // friction

  // max speed
  if(xVel > maxXVel) {
    xVel = maxXVel;
  } else if(xVel < -maxXVel) {
    xVel = -maxXVel;
  }

  // check for block collision
  // this should be improved by not zeroing the value but finding how much space is left to move
  // it should also have more thought into how it interacts with yVel
  for(int i = 0; i <= 1; i++) {
    for(int j = 0; j <= 1; j++) {
      int nextPosX = int(playerPosX + xVel + 0.5 * j + 0.25);
      int PosY = int(playerPosY + 0.95 * i);
      if(nextPosX >= 0 && nextPosX < levelW && PosY >= 0 && PosY < levelH) {
        if(tiles[PosY][nextPosX] == 3) {
          xVel = 0;
        }
      }
    }
  }

  if(yVel < 0 &&
     (tiles[int(playerPosY + yVel)][int(playerPosX + playerWidth / 2)] == 3 ||
      tiles[int(playerPosY + yVel)][int(playerPosX + 1 - playerWidth / 2)] == 3)) {
    yVel = 0;
  }

  if(onGround) {
    if(yVel > 0) {
      yVel = 0;
    }
  } else if(jumping) {
    yVel += 0.5 * gravity * deltaTime;
  } else {
    yVel += gravity * deltaTime;
  }

  if(yVel > 0.06) {
    yVel = 0.06;
  } else if(yVel < -0.08) {
    yVel = -0.08;
  }

  playerPosX += xVel * deltaTime;
  playerPosY += yVel * deltaTime;

  if(playerPosX < 0) {
    playerPosX = 0;
    xVel = 0;
  } else if(playerPosX > levelW - 1) {
    playerPosX = levelW - 1;
    xVel = 0;
  }

  if(playerPosY < 0) {
    playerPosY = 0;
    yVel = 0;
  } else if(playerPosY > levelH - 1) {
    playerPosY = levelH - 1;
    yVel = 0;
  }
}

int Platformer::draw() {
  bool quit = false;
  int steps = 1;

  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0; 
  double deltaTime = 0;

  while(!quit) {
    // https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

    bool jumping = false;
    bool lefting = false;
    bool righting = false;
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT) {
        quit = true;
      } else if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
          case 'q':
            quit = true;
            break;
          default:
            break;
        }
      }
    }

    // stack overflow SDL 2.0 Key repeat and delay
    // note that you need a SDL_PollEvent for this to be not empty
    const Uint8* keyStates = SDL_GetKeyboardState(nullptr);

    // add acceleration x to velocity
    if(keyStates[SDL_SCANCODE_LEFT]) {
      lefting = true;
    }
    if(keyStates[SDL_SCANCODE_RIGHT]) {
      righting = true;
    }
    if(keyStates[SDL_SCANCODE_UP] || keyStates[SDL_SCANCODE_SPACE]) {
      jumping = true;
    }

    physics(deltaTime, jumping, lefting, righting);

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

  // instead of doing anything fancy, just draw everything
  // works surprisingly well so far
  for(int i = 0; i < levelH; i++) {
    for(int j = 0; j < levelW; j++) {
      drawSprite(tiles[i][j], j * SCALED_SPRITE_WIDTH - screenX, i * SCALED_SPRITE_WIDTH - screenY, SPRITE_SCALE);
    }
  }

  drawSprite(12, int(playerPosX * SCALED_SPRITE_WIDTH) - screenX, int(playerPosY * SCALED_SPRITE_WIDTH) - screenY, SPRITE_SCALE);

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

  if(screenX < 0) {
    screenX = 0;
  } else if(screenX > levelW * SCALED_SPRITE_WIDTH - 640) {
    screenX = levelW * SCALED_SPRITE_WIDTH - 640;
  }

  if(screenY < 0) {
    screenY = 0;
  } else if(screenY > levelH * SCALED_SPRITE_WIDTH - 480) {
    screenY = levelH * SCALED_SPRITE_WIDTH - 480;
  }
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

        // set player position to the spawner block
        if(tiles[i][j] == 67) {
          playerPosX = j;
          playerPosY = i;
        }
      }
    }
  } else {
    SDL_Log("Error loading level data file\n");
    return;
  }
}
