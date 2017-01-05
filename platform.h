#if !defined(PLATFORM_H)
#define PLATFORM_H

/*
#include <stdint.h>
#include "timer.h"
#include "globals.h"
*/

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


struct game_button_state{
	bool8	EndedDown;
	bool8 	Changed;
};

struct game_input{
	int32				MouseX;
	int32				MouseY;

	union{
		game_button_state Buttons[13];
		struct{
			game_button_state 	e;
			game_button_state 	p;
			game_button_state 	r;
			game_button_state 	s;
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

struct game_memory{
	void 	*BaseAddress;
	uint64	Size;
	//bool32 	IsGameStateInitialized;
};

struct platform_state{
	SDL_Renderer*	Renderer;
	SDL_Window*		Window;
	SDL_Event 		Event;
	Timer 			FPS;
	SDL_Rect 		screen_outline{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	bool32 			Running;
	bool32			PlaybackStarted;
	uint32			FrameCount;
};

struct debug_read_file_result{
	void 	*Contents;
	uint32  ContentsSize;
};

internal debug_read_file_result DEBUGPlatformReadEntireFile(char *Filename);
internal void DEBUGPlatformFreeFileMemory(void *Memory);
internal bool32 DEBUGPLatformWriteEntireFile(char *Filename, uint64 MemorySize, void *Memory);

#endif //PLATFORM_H
