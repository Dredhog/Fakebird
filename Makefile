#g++ -std=c++0x -O3 -Wall -Wconversion -Wdouble-promotion  platform.cpp -o snake `sdl2-config --cflags --libs`

game:
	g++ -std=c++0x -O3 -Wall -Wconversion -Wdouble-promotion -fpic game.cpp -shared -o game.so

all:
	g++ -std=c++0x -O3 -Wall -Wconversion -Wdouble-promotion platform.cpp -o program `sdl2-config --cflags --libs`
	g++ -std=c++0x -O3 -Wall -Wconversion -Wdouble-promotion -fpic game.cpp -shared -o game.so
	./program
