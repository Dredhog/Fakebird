#if !defined(PLATFORM_H)
#define PLATFORM_H

struct platform_state{
	SDL_Renderer	*Renderer;
	SDL_Window		*Window;
	SDL_Event 		Event;
	offscreen_buffer OffscreenBuffer;
	Timer 			FPS;
	bool32 			Running;
	bool32			PlaybackStarted;
	uint32			FrameCount;
};

#define DEBUG_PROFILING 0
#if DEBUG_PROFILING 
#include "immintrin.h"
enum{
	DEBUG_UpdateAndRender,
	DEBUG_Simulation,
	DEBUG_Rendering,
	DEBUG_ClearOffscreenBuffer,
	DEBUG_StretchBitmapOrthogonaly,
	DEBUG_BlitOneBitmap,
};

struct debug_cycle_counter {
	uint64 CycleCount;
	uint64 Calls;
};

char DEBUG_TABLE_NAMES[][40] = {
	"UpdateAndRender",
	"Simulation",
	"Rendering",
	"ClearOffscreenBuffer",
	"StretchBitmapOrthogonaly",
	"BlitOneBitmap",
};

debug_cycle_counter DEBUG_CYCLE_TABLE[ArrayCount(DEBUG_TABLE_NAMES)];
#define BEGIN_TIMED_BLOCK(ID) uint64 StartCycleCount##ID = _rdtsc();
#define END_TIMED_BLOCK(ID) DEBUG_CYCLE_TABLE[DEBUG_##ID].CycleCount += _rdtsc() - StartCycleCount##ID; \
							DEBUG_CYCLE_TABLE[DEBUG_##ID].Calls++;
#else
#define BEGIN_TIMED_BLOCK(ID)
#define END_TIMED_BLOCK(ID)

#endif //DEGUB_PROFILING

#endif //PLATFORM_H
