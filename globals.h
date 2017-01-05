#ifndef ATTRIBUTES_INCLUDES_H_INCLUDED
#define ATTRIBUTES_INCLUDES_H_INCLUDED

//Math
static const float PI = 3.14159258979f;

//display
static const int SCREEN_WIDTH 	= 1260;
static const int SCREEN_HEIGHT 	= 800;
static const int SCREEN_DELAY 	= 10;
static const char TempName[] = "test_level";

//update
static const int FRAME_DURATION = 16;

//level
static const unsigned int LEVEL_COUNT = 5;
static const unsigned int LEVEL_MAX_WIDTH 	= 25;
static const unsigned int LEVEL_MAX_HEIGHT 	= 25;

//block
static unsigned int BLOCK_WIDTH_IN_PIXELS 		= 60;
static unsigned int BLOCK_HEIGHT_IN_PIXELS 		= 60;

//Snakebirds
static const unsigned int SNAKE_MAX_COUNT 		= 10;
static const unsigned int SNAKE_MAX_LENGTH 		= 20;
static const unsigned int SNAKE_ID_OFFSET 		= 5;
#endif // ATTRIBUTES_INCLUDES_H_INCLUDED
