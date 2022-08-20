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
    int SPRITE_WIDTH = 16;
    int SPRITE_SCALE = 4;
    int SCALED_SPRITE_WIDTH = SPRITE_SCALE * SPRITE_WIDTH;
    int SHEET_HEIGHT = 5;
    int SHEET_WIDTH = 16;
    // This is correct most of the time
    // it will be 1 wider than it should if the sprite perfectly divides the width
    int TILED_SCREEN_WIDTH = (640 / SCALED_SPRITE_WIDTH) + 1;
    int TILED_SCREEN_HEIGHT = (480 / SCALED_SPRITE_WIDTH) + 1;
    SDL_Texture* spriteSheet;

    void drawSprite(int sprite, int x, int y, int scale = 1);

    virtual int setup();
    virtual int draw();
    virtual int clean();

    // level data
    void readLevel(std::string filename);
    void saveLevel(std::string filename);
    int levelW, levelH;
    int** tiles;

    int screenPosX;
    int screenPosY;
    int tileX;
    int tileY;
    int screenTileX;
    int screenTileY;

    void renderScreen(int brush);
    void tileFromScreen();
    int currentTile;
    void moveScreenToTile(int tx, int ty, int steps);
    void setScale(int scale);
    int getScale();
};

#endif
