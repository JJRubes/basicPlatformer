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
  xRemainder = 0;
  yRemainder = 0;
  vXRemainder = 0;
  vYRemainder = 0;
  screenXRemainder = 0;
  screenYRemainder = 0;

  // set colour to magenta to make it obvious
  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
  return 0;
}

bool Platformer::collidable(int tile) {
  return tile == 3 || tile == 6 || tile == 28 || tile == 29 || tile == 30 || tile == 34 || tile == 50
    || tile == 51;
}

bool Platformer::collides(int x, int y) {
  // check for collision with edge
  if(x + leftPlayerOffset < 0 || (x + rightPlayerOffset) / SPRITE_WIDTH >= levelW
      || y < 0 || (y + SPRITE_WIDTH - 1) / SPRITE_WIDTH >= levelH)
    return true;

  // top left
  bool collision = collidable(tiles[y / SPRITE_WIDTH][(x + leftPlayerOffset) / SPRITE_WIDTH]);
  // top right
  collision = collision || collidable(tiles[y / SPRITE_WIDTH][(x + rightPlayerOffset) / SPRITE_WIDTH]);
  // bottom left
  collision = collision || collidable(tiles[(y + SPRITE_WIDTH - 1) / SPRITE_WIDTH][(x + leftPlayerOffset) / SPRITE_WIDTH]);
  // bottom right
  collision = collision || collidable(tiles[(y + SPRITE_WIDTH - 1) / SPRITE_WIDTH][(x + rightPlayerOffset) / SPRITE_WIDTH]);
  return collision;
}

void Platformer::physics(double deltaTime,
    bool jumping, bool lefting, bool righting) {
  double acc = 0.0005;
  double jump = 100;
  double gravity = 0.0005;
  double maxXVel = 0.2;
  double maxYVel = 0.25;
  double sustainedJump = 0.33;

  // this assumes that the player is not in a wall
  bool onGround = collides(posX, posY + SPRITE_WIDTH);

  // yVel check seems redundant now that everything is pixel aligned
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

  if(!lefting && !righting) {
    if(xVel > 0) {
      if(onGround) {
        xVel -= acc * deltaTime;
      } else {
        xVel -= 0.75 * acc * deltaTime;
      }
      if(xVel < 0)
        xVel = 0;
    }
    if(xVel < 0) {
      if(onGround) {
        xVel += acc * deltaTime;
      } else {
        xVel += 0.75 * acc * deltaTime;
      }
      if(xVel > 0)
        xVel = 0;
    }
  }

  // max speed
  if(xVel > maxXVel) {
    xVel = maxXVel;
  } else if(xVel < -maxXVel) {
    xVel = -maxXVel;
  }

  // check for block collision
  // based off of celeste collision
  // https://maddythorson.medium.com/celeste-and-towerfall-physics-d24bd2ae0fc5
  xRemainder += xVel;
  int move = round(xRemainder);

  if(move != 0) {
    xRemainder -= move;
    vXRemainder = xRemainder;
    int sign = move > 0 ? 1 : -1;

    while(move != 0) {
      if(collides(posX + sign, posY)) {
        // collision
        xVel = 0;
        vXRemainder = 0;
        break;
      } else {
        posX += sign;
        move -= sign;
      }
    }
    // vXRemainder = xRemainder;
  } else {
    int sign = xVel > 0 ? 1 : -1; // maybe this is the culprit for the right wall flicking?
    if(collides(posX + sign, posY)) {
      vXRemainder = 0; // this might need to include a positive/negative check for left or right side
    } else {
      vXRemainder = xRemainder;
    }
  }


  // if(jumping) {
  //   yVel += 0.5 * gravity * deltaTime;
  // } else {
  yVel += gravity * deltaTime;
  // }

  if(yVel > maxYVel) {
    yVel = maxYVel;
  } else if(jumping) {
    if(yVel < -sustainedJump) {
      yVel = -sustainedJump;
    }
  } else if(yVel < -maxYVel) {
    yVel = -maxYVel;
  }

  yRemainder += yVel;
  move = round(yRemainder);

  if(move != 0) {
    yRemainder -= move;
    vYRemainder = yRemainder;
    int sign = move > 0 ? 1 : -1;

    while(move != 0) {
      if(collides(posX, posY + sign)) {
        // collision
        yVel = 0;
        vYRemainder = 0;
        break;
      } else {
        posY += sign;
        move -= sign;
      }
    }
  } else {
    int sign = yVel > 0 ? 1 : -1;
    if(collides(posX, posY + sign)) {
      vYRemainder = 0;
    } else {
      vYRemainder = yRemainder;
    }
  }
}

int Platformer::draw() {
  bool quit = false;

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
    // TODO there is a small window where this will drop input
    if(keyStates[SDL_SCANCODE_UP] || keyStates[SDL_SCANCODE_SPACE]) {
      jumping = true;
    }

    physics(deltaTime, jumping, lefting, righting);

    // this causes some artifacts, including screen wobble
    // but if I do something like posX + xRemainder the pixel snapping becomes jarring
    // and against walls and the floor the screen vibrates
    // moveScreenToPosition(double(posX) / SPRITE_WIDTH, double(posY) / SPRITE_WIDTH, 100);
    moveScreenToPosition(posX + vXRemainder, posY + vYRemainder, 20);
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

  // drawSprite(12, posX * SPRITE_SCALE - screenX, posY * SPRITE_SCALE - screenY, SPRITE_SCALE);
  drawSprite(12, (posX + vXRemainder) * SPRITE_SCALE - screenX, (posY + vYRemainder) * SPRITE_SCALE - screenY, SPRITE_SCALE);

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

// TODO bug that means that it doesn't follow the player for a small amount to the right of the centre of screen
// move the screen so that it is centred on a given tile
void Platformer::moveScreenToPosition(double x, double y, int step) {
  // find the top left of the player
  int playerWorldPosX = int(x * SPRITE_SCALE);
  int playerWorldPosY = int(y * SPRITE_SCALE);

  // subtract half the screen width to centre the top-left
  // then add half of the sprite width to centre on the middle of the tile
  int centredX = playerWorldPosX - 640 / 2 + SCALED_SPRITE_WIDTH;
  int centredY = playerWorldPosY - 480 / 2 + SCALED_SPRITE_WIDTH / 2;
  screenXRemainder += double(centredX - screenX) / double(step);
  int moveX = round(screenXRemainder);
  screenXRemainder -= moveX;
  screenX += moveX;
  screenYRemainder += (1.0 / step) * (centredY - screenY);
  int moveY = round(screenYRemainder);
  screenYRemainder -= moveY;
  screenY += moveY;

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
          posX = j * SPRITE_WIDTH;
          posY = i * SPRITE_WIDTH;
          moveScreenToPosition(posX, posY, 1);
        }
      }
    }
  } else {
    SDL_Log("Error loading level data file\n");
    return;
  }
}
