#if !defined(TYPES_H)
#define TYPES_H

/*
#include "vec2.h"
#include "platform.h"
#include "globals.h"
*/

struct color{
	union{
		struct{
			uint8 R;
			uint8 G;
			uint8 B;
			uint8 A;
		};
		uint32 All;
	};
};

static color SNAKE_COLORS[3][2] = 	{{color{255, 10, 10, 255}, color{255, 76, 48, 255}},
									{color{0, 76, 255, 255}, color{0, 136, 255, 255}},
									{color{24, 209, 31, 255}, color{71, 219, 72, 255}}};

static color TILE_COLORS[5] = {	{154, 120, 55, 255},
							   	{200, 200, 55, 255},
							   	{154, 155, 155, 255},
								{120, 50, 100, 255},
								{255, 50, 200, 255}};

static color TILEMAP_BACKGROUND_COLORS[LEVEL_MAX_LAYER_COUNT] = {	{24, 255, 31, 255}, 
																	{200, 60, 255, 255},
																	{200, 100, 40, 255}};
struct snake_part{
	vec2i 		GridP;	//Cartesian coord (first quadrant)
};

struct snake{
	uint32 		Length;
	color		Color0;
	color		Color1;
	snake_part 	Parts[SNAKE_MAX_LENGTH];
	uint32		SnakeID;
};

enum basic_tyle_type{
	Tile_Type_Empty,
	Tile_Type_Solid,
	Tile_Type_Fruit,
	Tile_Type_Spikes,
	Tile_Type_Goal,
};

struct tile{
	SDL_Rect Source;
};

struct level{
	uint32 Occupancy[LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	tile Tiles[LEVEL_MAX_LAYER_COUNT][LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	uint32 Width;
	uint32 Height;
	snake  Snakes[SNAKE_MAX_COUNT];
	uint32 SnakeCount;
	int32 ForegroundLayerIndex;
};

enum game_mode{
	Game_Mode_Play,
	Game_Mode_Edit,
	Game_Mode_Tile,
};

struct game_state{
	snake	*Player;

	level 	Level;
	uint32	LevelIndex;
	level 	Levels[LEVEL_COUNT];

	snake 	*VisitedSnakes[SNAKE_MAX_COUNT];
	uint8 	Visited[SNAKE_MAX_COUNT];
	uint32 	VisitedCount;

	uint32	FruitRemaining;
	vec2i	PortalP;

	uint32 	ActiveBrush;
	vec2i 	MouseGridP;
	vec2i	MouseSpriteSheetP;

	tile	ActiveTileBrush;
	SDL_Rect TileWindowRect;
	bool32 TileWindowActive;
	int32 ActiveLayerIndex;

	SDL_Texture	*TileTexture;
	int32 TextureWidth;
	int32 TextureHeight;

	game_mode Mode;
	uint32	MagicChecksum;
};

static void
UpdateAndRender(game_memory *Memory, platform_state *Platform, game_input *Input);

#define DEBUG_PROFILING 0

#if DEBUG_PROFILING 
enum{
	DEBUG_UpdateAndRender,
	DEBUG_Simulation,
	DEBUG_SwapBuffer,
	DEBUG_Last,
};
struct debug_cycle_counter {
	uint64 CycleCount;
	uint64 Calls;
};
debug_cycle_counter DEBUG_CYCLE_TABLE[DEBUG_Last] = {};
#define BEGIN_TIMED_BLOCK(ID) uint64 StartCycleCount##ID = _rdtsc();
#define END_TIMED_BLOCK(ID) DEBUG_CYCLE_TABLE[DEBUG_##ID].CycleCount += _rdtsc() - StartCycleCount##ID; \
							DEBUG_CYCLE_TABLE[DEBUG_##ID].Calls++;
char DEBUG_TABLE_NAMES[][40] = {
	"UpdateAndRender",
	"Simulation",
	"SwapBuffer",
};
#else
#define BEGIN_TIMED_BLOCK(ID)
#define END_TIMED_BLOCK(ID)
#endif //DEGUB_PROFILING




#endif //TYPES_H
