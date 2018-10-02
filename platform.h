#if !defined(PLATFORM_H)
#define PLATFORM_H

#define Kilobytes(Count) ((Count)*1024ll)
#define Megabytes(Count) (Kilobytes(Count)*1024ll)
#define Gigabytes(Count) (Megabytes(Count)*1024ll)

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

#endif //PLATFORM_H
