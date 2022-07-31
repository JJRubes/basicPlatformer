#ifndef EDITOR
#define EDITOR

// tileset from
// https://opengameart.org/content/16x16-dark-tech-base-tileset
// because I didn't have the want to make my own

#include"game.h"

class Editor : public Game {
  public:
    Editor();

  private:
    const static int SPRITE_WIDTH = 16;
    const static int SPRITE_SCALE = 3;
    const static int SCALED_SPRITE_WIDTH = SPRITE_SCALE * SPRITE_WIDTH;
    const static int SHEET_HEIGHT = 5;
    const static int SHEET_WIDTH = 16;
    // This is correct most of the time
    // it will be 1 wider than it should if the sprite perfectly divides the width
    const static int TILED_SCREEN_WIDTH = ((640 / 2) / SCALED_SPRITE_WIDTH) + 1;
    const static int TILED_SCREEN_HEIGHT = ((480 / 2) / SCALED_SPRITE_WIDTH) + 1;
    SDL_Texture* spriteSheet;

    void drawSprite(int sprite, int x, int y, int scale = 1);

    virtual int setup();
    virtual int draw();
    virtual int clean();
};

#endif
