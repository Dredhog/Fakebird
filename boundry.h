#if !defined(COMMON_H)
#define COMMON_H 

#include <stdint.h>

#define ArrayCount(Array) sizeof((Array))/sizeof(Array[0])

#define internal static
#define local_persist static
#define global_variable static

////PRECISE TYPES/////
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
///////////////////////

////COMMON DEFINITIONS FOR GAME AND PLATFORM
struct SDL_Texture;

struct offscreen_buffer{
	SDL_Texture *Texture;
	void 		*Memory;
	int32 		Width;
	int32 		Height;
	int32 		Pitch;
	int32 		BytesPerPixel;
};

struct loaded_bitmap{
	void *Texels;
	int32 Width;
	int32 Height;
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
		game_button_state Buttons[17];
		struct{
			game_button_state 	e;
			game_button_state 	g;
			game_button_state 	m;
			game_button_state 	n;
			game_button_state 	o;
			game_button_state 	p;
			game_button_state 	r;
			game_button_state 	s;
			game_button_state 	t;
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
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(platform_read_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void  name(debug_read_file_result FileHandle)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(platform_free_file_memory);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(char *Filename, uint64 MemorySize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(platform_write_entire_file);

#define DEBUG_PLATFORM_LOAD_BITMAP_FROM_FILE(name) loaded_bitmap  name(char *Filename)
typedef DEBUG_PLATFORM_LOAD_BITMAP_FROM_FILE(platform_load_bitmap_from_file);

struct platform_services{
	platform_read_entire_file 		*ReadEntireFile;
	platform_free_file_memory 		*FreeFileMemory;
	platform_write_entire_file 		*WriteEntireFile;
	platform_load_bitmap_from_file	*LoadBitmapFromFile;
};

#define DEBUG_PROFILING 1
#if DEBUG_PROFILING  
#include <x86intrin.h>
enum{
	DEBUG_GameUpdateAndRender,
	DEBUG_UpdateLogic,
	DEBUG_ClearOffscreenBuffer,
	DEBUG_StretchBitmapOrthogonaly,
	DEBUG_FillRect,
	DEBUG_DrawRectOutline,
	DEBUG_DrawTriangle,
};

struct debug_cycle_counter {
	uint64 CycleCount;
	uint64 Calls;
};

char DEBUG_TABLE_NAMES[][40] = {
	"GameUpdateAndRender",
	"UpdateLogic",
	"ClearOffscreenBuffer",
	"StretchBitmapOrthogonaly",
	"FillRect",
	"DrawRectOutline",
	"DrawTriangle",
};

debug_cycle_counter *GLOBAL_DEBUG_CYCLE_TABLE;
#define INIT_DEBUGING() GLOBAL_DEBUG_CYCLE_TABLE = Memory->DEBUG_CYCLE_TABLE;
#define CLEAR_DEBUG_CYCLE_TABLE() for(uint32 i = 0; i < ArrayCount(DEBUG_TABLE_NAMES); i++){ GLOBAL_DEBUG_CYCLE_TABLE[i] = {};}
#define BEGIN_TIMED_BLOCK(ID) uint64 StartCycleCount##ID = _rdtsc();
#define END_TIMED_BLOCK(ID) GLOBAL_DEBUG_CYCLE_TABLE[DEBUG_##ID].CycleCount += _rdtsc() - StartCycleCount##ID; \
							GLOBAL_DEBUG_CYCLE_TABLE[DEBUG_##ID].Calls++;
#else
#define INIT_DEBUGING()
#define CLEAR_DEBUG_CYCLE_TABLE()
#define BEGIN_TIMED_BLOCK(ID)
#define END_TIMED_BLOCK(ID)
#endif //DEGUB_PROFILING

struct game_memory{
	void 	*BaseAddress;
	uint64	Size;
	platform_services PlatformServices;
#if DEBUG_PROFILING
	debug_cycle_counter DEBUG_CYCLE_TABLE[ArrayCount(DEBUG_TABLE_NAMES)];
#endif
};


#endif //COMMON_H 
