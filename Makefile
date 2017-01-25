all:
	g++ -std=c++0x  -Wall -Wconversion -Wdouble-promotion  -g platform.cpp -o snake `sdl2-config --cflags --libs`
	./snake
run: 
	./snake
