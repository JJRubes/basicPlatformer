#ifndef GAME
#define GAME
#include<SDL.h>
#include<SDL_image.h>
#include<string>

class Game {
  public:
    Game();
    
    // actually use the dang thing
    void run();


  protected:
    int init();
    void quit();
    SDL_Texture* loadTexture(std::string path);

    virtual int setup();
    virtual int draw() = 0;

    // define your own clean function to clean up everything you gunked up
    virtual int clean() = 0;

    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif
