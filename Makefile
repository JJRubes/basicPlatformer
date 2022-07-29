FLAGSANDLIBS=`sdl2-config --cflags --libs` -lSDL2_image -lpng
main: main.cpp game.o platformer.o
	g++ $^ $(FLAGSANDLIBS) -o main

level: levelMain.cpp game.o editor.o
	g++ $^ $(FLAGSANDLIBS) -o level

game.o: game.cpp game.h
	g++ -c game.cpp $(FLAGSANDLIBS) -o game.o

%.o: %.cpp %.h
	g++ -c $< $(FLAGSANDLIBS) -o $@
