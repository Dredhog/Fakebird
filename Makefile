comp := clang++-5.0
#-fcolor-diagnostics

common_flags=-std=c++14 -O3 -Wall -Wdouble-promotion -Wno-missing-braces -Wno-unused-function -Wno-unused-variable

game:
	@$(comp) -std=c++11 -O3 -fpic game.cpp -fcolor-diagnostics -shared -o game.so

all:
	@$(comp) ${common_flags} -fpic game.cpp -shared -o game.so
	@$(comp) ${common_flags} platform.cpp -o program `sdl2-config --cflags --libs`
	./program
