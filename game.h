#if !defined(TYPES_H)
#define TYPES_H

#define ArrayCount(Array) sizeof((Array))/sizeof(Array[0])

#define internal static
#define local_persist static
#define global_variable static

#define Kilobytes(Count) ((Count)*1024ll)
#define Megabytes(Count) (Kilobytes(Count)*1024ll)
#define Gigabytes(Count) (Megabytes(Count)*1024ll)

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8 bool8;
typedef int32 bool32;

typedef float real32;
typedef double real64;

union rectangle{
	struct{
		vec2i MinP;
		vec2i MaxP;
	};
	struct{
		int32 MinX;
		int32 MinY;
		int32 MaxX;
		int32 MaxY;
	};
	int32 All[4];
};

struct snake_part{
	vec2i 	GridP;	//Cartesian coord (first quadrant)
};

enum transition_type{
	Transition_Type_None,
	Transition_Type_Slide,
	Transition_Type_GotPushed,
	Transition_Type_Fall,
	Transition_Type_Teleported,
	Transition_Type_FinishedLevel,
};

struct transition_slid{
	transition_type Type;
	vec2i NewHeadP;
};
struct transition_got_pushed{
	transition_type Type;
	vec2i Direction;
};
struct transition_fell{
	transition_type Type;
	vec2i FallRadius;
};

union transition{
	transition_type 		Type;
	transition_slid 		Slide;
	transition_fell			Fell;
	transition_got_pushed 	GotPushed;
};

struct snake{
	int32 		Length;
	uint32		PaletteIndex;
	snake_part 	Parts[SNAKE_MAX_LENGTH];
	uint32		SnakeID;
	transition	Transition;
};

enum basic_tyle_type{
	Tile_Type_Empty,
	Tile_Type_Solid,
	Tile_Type_Fruit,
	Tile_Type_Spikes,
	Tile_Type_Goal,
};

struct tile{
	rectangle Source;
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

struct marked_snakes{
	snake 	*MarkedSnakes[SNAKE_MAX_COUNT];
	uint8 	IsMarked[SNAKE_MAX_COUNT];
	int32 	Count;
};

struct SDL_Texture;

struct offscreen_buffer{
	SDL_Texture *Texture;
	void 		*Memory;
	int32 		Width;
	int32 		Height;
	int32		Pitch;
	int32 		BytesPerPixel;
};

struct loaded_bitmap{
	void *Texels;
	int32 Width;
	int32 Height;
};

struct game_state{
	snake	*Player;

	level 	Level;
	uint32	LevelIndex;
	uint32 	LevelCount;
	char 	CurrentLevelName[2];
	bool	Transitioning;
	real32 	t;

	marked_snakes MarkedSnakes;

	uint32	FruitRemaining;
	vec2i	PortalP;

	uint32 	ActiveBrush;
	vec2i 	MouseGridP;

	tile	ActiveTileBrush;
	rectangle SpriteAtlasRect;
	bool32 SpriteAtlasActive;
	int32 ActiveLayerIndex;

	loaded_bitmap SpriteAtlas;

	game_mode Mode;
	uint32	MagicChecksum;
};



struct debug_read_file_result{
	void 	*Contents;
	uint32  ContentsSize;
};

struct game_button_state{
	bool8	EndedDown;
	bool8 	Changed;
};

struct game_input{
	int32				MouseX;
	int32				MouseY;

	union{
		game_button_state Buttons[14];
		struct{
			game_button_state 	e;
			game_button_state 	p;
			game_button_state 	r;
			game_button_state 	s;
			game_button_state 	t;
			game_button_state 	n;
			game_button_state 	LeftCtrl;
			game_button_state 	Space;
			game_button_state 	ArrowUp;
			game_button_state 	ArrowDown;
			game_button_state 	ArrowLeft;
			game_button_state 	ArrowRight;
			game_button_state 	MouseLeft;
			game_button_state 	MouseRight;
			game_button_state 	Escape;
		};
	};
};
struct game_memory;

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory *Memory, offscreen_buffer OffscreenBuffer, game_input *Input)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub){
}

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result  name(char *Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void  name(void *Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(char *Filename, uint64 MemorySize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#define DEBUG_PLATFORM_LOAD_BITMAP_FROM_FILE(name) loaded_bitmap  name(char *Filename)
typedef DEBUG_PLATFORM_LOAD_BITMAP_FROM_FILE(debug_platform_load_bitmap_from_file);

struct platform_service_v_table{
	debug_platform_read_entire_file 		*DEBUGPlatformReadEntireFile;
	debug_platform_free_file_memory 		*DEBUGPlatformFreeFileMemory;
	debug_platform_write_entire_file 		*DEBUGPLatformWriteEntireFile;
	debug_platform_load_bitmap_from_file	*DEBUGPlatformLoadBitmapFromFile;
};

struct game_memory{
	void 	*BaseAddress;
	uint64	Size;
	platform_service_v_table PlatformServices;
};

#endif //TYPES_H
