main: main.cpp
	g++ main.cpp `sdl2-config --cflags --libs` -lSDL_image -lpng -o main
