#ifndef CONSTANTS_GAME_H
#define CONSTANTS_GAME_H

//level
static const int LEVEL_MAX_WIDTH	= 25;
static const int LEVEL_MAX_HEIGHT 	= 20;
static const int LEVEL_MAX_COUNT 	= 50;

static const unsigned int EDIT_BRUSH_COUNT = 10;

//Layers
static const int LEVEL_MAX_LAYER_COUNT = 3;
static const int LEVEL_ICON_SIZE_IN_PIXELS = 100;

//Bitmaps
static const int BITMAP_MAX_COUNT = 10;

//Tiles
static const int MOBILE_TILE_MAX_COUNT = 5;
static const int ANIMATED_TILE_MAX_COUNT = 40;


//block
static int DEST_TILE_SIZE_IN_PIXELS 	= 35;

//Snakebirds
static const int SNAKE_MAX_COUNT 	= 10;
static const int SNAKE_MAX_LENGTH 	= 30;
static const int SNAKE_ID_OFFSET 	= 7;

#define TELEPORTATION_MID_AIR 1
#define COMPLETE_AND_DELETE_MID_AIR 1
#define DELETE_ON_REACHING_GOAL 1
#define GRAVITY 1

#endif // CONSTANTS_GAME_H
