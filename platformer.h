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

    float SPRITE_SCALE = 3;
    float SCALED_SPRITE_WIDTH = SPRITE_SCALE * SPRITE_WIDTH;
    int TILED_SCREEN_WIDTH = (640 / SCALED_SPRITE_WIDTH) + 1;
    int TILED_SCREEN_HEIGHT = (480 / SCALED_SPRITE_WIDTH) + 1;

    void drawSprite(int sprite, int x, int y, int scale = 1);

    virtual int setup();
    virtual int draw();
    virtual int clean();

    // level data
    void readLevel(std::string filename);
    void saveLevel(std::string filename);
    int levelW, levelH;
    int** tiles;

    int leftPlayerOffset = SPRITE_WIDTH / 4;
    int rightPlayerOffset = 3 * SPRITE_WIDTH / 4;
    int posX;
    int posY;
    double xRemainder;
    double yRemainder;
    double vXRemainder;
    double vYRemainder;
    double xVel;
    double yVel;
    void physics(double deltaTime, bool jumping, bool lefting, bool righting);
    bool collidable(int tile);
    bool collides(int posX, int posY);
    int screenX;
    int screenY;
    double screenXRemainder;
    double screenYRemainder;
    int screenTileX;
    int screenTileY;

    void renderScreen();
    void tileFromScreen();
    void moveScreenToPosition(double x, double y, int steps);
};

#endif
