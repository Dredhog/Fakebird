#ifndef ATTRIBUTES_INCLUDES_H_INCLUDED
#define ATTRIBUTES_INCLUDES_H_INCLUDED

//Math
static const float PI = 3.14159258979f;

//display
static const int SCREEN_WIDTH 	= 1111;
static const int SCREEN_HEIGHT 	= 900;
static const int SCREEN_DELAY 	= 1;


//update
static const int FRAME_DURATION = 16;

//level
static const char TempName[] = "test_level";
static const unsigned int LEVEL_COUNT = 5;
static const int LEVEL_MAX_WIDTH 	= 25;
static const int LEVEL_MAX_HEIGHT 	= 25;
static const unsigned int EDIT_BRUSH_COUNT = 8;
//Layers
static const int LEVEL_MAX_LAYER_COUNT = 3;

//block
static int BLOCK_WIDTH_IN_PIXELS 		= 50;
static int BLOCK_HEIGHT_IN_PIXELS 		= 50;

static int SOURCE_BLOCK_WIDTH_IN_PIXELS = 77;
static int SOURCE_BLOCK_HEIGHT_IN_PIXELS = 77;

//Snakebirds
static const unsigned int SNAKE_MAX_COUNT 		= 10;
static const unsigned int SNAKE_MAX_LENGTH 		= 30;
static const unsigned int SNAKE_ID_OFFSET 		= 5;
#endif // ATTRIBUTES_INCLUDES_H_INCLUDED
