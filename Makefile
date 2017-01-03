all:
	g++ -std=c++0x -O3 -Wall -Wconversion -Wdouble-promotion  -g platform.cpp -o snakebird `sdl2-config --cflags --libs`
	./snakebird
run: 
	./snakebird
