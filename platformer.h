#ifndef PLATFORMER
#define PLATFORMER

// tileset from
// https://opengameart.org/content/16x16-dark-tech-base-tileset
// because I didn't have the want to make my own

#include"game.h"

class Platformer : public Game {
  public:
    Platformer();

  private:
    const static int SPRITE_WIDTH = 16;
    const static int SHEET_HEIGHT = 5;
    const static int SHEET_WIDTH = 16;
    SDL_Texture* spriteSheet;

    void drawSprite(int sprite, int x, int y, int scale = 1);

    virtual int setup();
    virtual int draw();
    virtual int clean();
};

#endif
