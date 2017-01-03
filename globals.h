#ifndef ATTRIBUTES_INCLUDES_H_INCLUDED
#define ATTRIBUTES_INCLUDES_H_INCLUDED

//Math
static const float PI = 3.14159258979f;

//display
static const int SCREEN_WIDTH 	= 1080;
static const int SCREEN_HEIGHT 	= 720;
static const int SCREEN_DELAY 	= 10;

//update
static const int FRAME_DURATION = 16;


//level
static const unsigned int LEVEL_MAX_WIDTH 	= 22;
static const unsigned int LEVEL_MAX_HEIGHT 	= 22;

static const unsigned int EXAMPLE_LEVEL_HEIGHT 	= 8;
static const unsigned int EXAMPLE_LEVEL_WIDTH 	= 22;
static const unsigned int EXAMPLE_BOARD[EXAMPLE_LEVEL_HEIGHT][EXAMPLE_LEVEL_WIDTH] =	 {	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																							{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																							{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																							{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																							{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
																							{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0},
																							{0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 3, 0, 0, 0, 1, 1},
																							{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

//block
static unsigned int BLOCK_WIDTH_IN_PIXELS 		= 60;
static unsigned int BLOCK_HEIGHT_IN_PIXELS 		= 60;
//static unsigned int BLOCK_SPACING_IN_PIXELS 	= 70;

//Snakebirds
static const unsigned int SNAKE_MAX_COUNT 		= 10;
static const unsigned int SNAKE_MAX_LENGTH 		= 20;
static const unsigned int SNAKE_ID_OFFSET 		= 5;
#endif // ATTRIBUTES_INCLUDES_H_INCLUDED
