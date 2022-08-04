#ifndef TILESHEET
#define TILESHEET
#include<string>

class TileSheet {
  public:
    TileSheet(std::string filename, int tileWidth, int scale, int windowWidth, int windowHeight);
  private:
    int scale;
    int tileWidth;
    SDL_Texture* tileSheet;
    int totalWidth, totalHeight;
};

#endif
