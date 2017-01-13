#include <SDL.h>
#include <random>
#include "vec2.h"
#include "assert.h"

#include <stdint.h>
#include "globals.h"
#include "timer.h"
#include "timer.cpp"
#include "platform.h"
#include "game.h"
#include "game.cpp"
#include "immintrin.h"
#include "circular_buffer.h"
#include <sys/mman.h>
//#include <Windows.h>

//Files
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

internal bool32
InitPlatform(platform_state *Platform)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Platform: SDL Initialization error: %s\n", SDL_GetError());
		return false;
	}
	if ((Platform->Window = SDL_CreateWindow("Snakebird ripoff", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)) == NULL) {
		printf("Platform: Window creation error: %s\n", SDL_GetError());
		return false;
	}
	if ((Platform->Renderer = SDL_CreateRenderer(Platform->Window, -1, SDL_RENDERER_ACCELERATED)) == NULL) {
		printf("Platform: Renderer creation error: %s\n", SDL_GetError());
		return false;
	}
	Platform->Running = true;
	return true;
}

uint32 SafeTruncateUint64(Uint64 Value){
	assert(Value <= 0xFFFFFFFF);
	uint32 Result = (uint32)Value;
	return Result;
}


internal debug_read_file_result
DEBUGPlatformReadEntireFile(char *Filename){
	debug_read_file_result Result = {};
	int FileHandle = open(Filename, O_RDONLY);
	if(FileHandle == -1){
		return Result;
	}

	struct stat FileStatus;
	if (fstat(FileHandle, &FileStatus) == -1){
		close(FileHandle);
		return Result;
	}
	Result.ContentsSize = SafeTruncateUint64(FileStatus.st_size);

	Result.Contents = malloc(Result.ContentsSize);
	if(!Result.Contents){
		Result.ContentsSize = 0;
		close(FileHandle);
		return Result;
	}
	
	uint64 BytesToRead = Result.ContentsSize;
	uint8 *NextByteLocation = (uint8*)Result.Contents;
	while(BytesToRead){
		int64 BytesRead = read(FileHandle, NextByteLocation, BytesToRead);
		if(BytesRead == -1 ){
			free(Result.Contents);
			Result.Contents = 0;
			Result.ContentsSize = 0;
			close(FileHandle);
			return Result;
		}
		BytesToRead -= BytesRead;
		NextByteLocation += BytesRead;
	}
	close(FileHandle);
	return Result;
}

internal void
DEBUGPlatformFreeFileMemory(void *Memory){
	free(Memory);
}

internal bool32 DEBUGPLatformWriteEntireFile(char *Filename, uint64 MemorySize, void *Memory){
	int FileHandle = open(Filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if(FileHandle == -1){
		return false;
	}

	uint64 BytesToWrite = MemorySize;
	uint8 *NextByteLocation = (uint8*)Memory;
	while(BytesToWrite){
		int64 BytesWritten = write(FileHandle, NextByteLocation, BytesToWrite);
		if(BytesWritten == -1){
			close(FileHandle);
			return false;
		}
		BytesToWrite -= BytesWritten;
		NextByteLocation += BytesWritten;
	}
	close(FileHandle);
	return true;
}

internal void
AllocateGameMemory(game_memory *GameMemory) {
	//GameMemory->Size = Kilobytes(200);
	GameMemory->Size = Megabytes(1);
	GameMemory->BaseAddress = (void*)Gigabytes(10);
	//LPVOID BaseAddress = (LPVOID*)Gigabytes(10);
	GameMemory->BaseAddress = mmap(GameMemory->BaseAddress, GameMemory->Size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	//GameMemory->BaseAddress = VirtualAlloc(BaseAddress, GameMemory->Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

internal SDL_Texture *DEBUGPlatformLoadImageFromFile(SDL_Renderer *Renderer, char *FileName){
	SDL_Texture *Result = {};
	SDL_Surface *ImageSurface = SDL_LoadBMP(FileName);

	if(ImageSurface){
		Result = SDL_CreateTextureFromSurface(Renderer, ImageSurface);
	}else{
		printf("Platform: Window creation error: %s\n", SDL_GetError());
	}
	return Result;
}

internal bool32
ProcessInput(game_input *OldInput, game_input *NewInput, SDL_Event* Event)
{
	*NewInput = *OldInput;
	while (SDL_PollEvent(Event) != 0)
	{
		switch (Event->type) {
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if (Event->key.keysym.sym == SDLK_ESCAPE) {
				NewInput->Escape.EndedDown = true;
				return false;
			} else if (Event->key.keysym.sym == SDLK_SPACE) {
				NewInput->Space.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_LCTRL) {
				NewInput->LeftCtrl.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_e) {
				NewInput->e.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_p) {
				NewInput->p.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_r) {
				NewInput->r.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_s) {
				NewInput->s.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_t) {
				NewInput->t.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_n) {
				NewInput->n.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_UP) {
				NewInput->ArrowUp.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_DOWN) {
				NewInput->ArrowDown.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_RIGHT) {
				NewInput->ArrowRight.EndedDown = true;
			} else if (Event->key.keysym.sym == SDLK_LEFT) {
				NewInput->ArrowLeft.EndedDown = true;
			}
			break;
		case SDL_KEYUP:
			if (Event->key.keysym.sym == SDLK_SPACE) {
				NewInput->Space.EndedDown = false;
			}else if (Event->key.keysym.sym == SDLK_LCTRL) {
				NewInput->LeftCtrl.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_e) {
				NewInput->e.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_p) {
				NewInput->p.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_r) {
				NewInput->r.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_s) {
				NewInput->s.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_t) {
				NewInput->t.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_n) {
				NewInput->n.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_UP) {
				NewInput->ArrowUp.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_DOWN) {
				NewInput->ArrowDown.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_RIGHT) {
				NewInput->ArrowRight.EndedDown = false;
			} else if (Event->key.keysym.sym == SDLK_LEFT) {
				NewInput->ArrowLeft.EndedDown = false;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (Event->button.button == SDL_BUTTON_LEFT) {
				NewInput->MouseLeft.EndedDown = true;
			} else if (Event->button.button == SDL_BUTTON_RIGHT) {
				NewInput->MouseRight.EndedDown = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (Event->button.button == SDL_BUTTON_LEFT) {
				NewInput->MouseLeft.EndedDown = false;
			} else if (Event->button.button == SDL_BUTTON_RIGHT) {
				NewInput->MouseRight.EndedDown = false;
			}
			break;
		}
	}
	SDL_GetMouseState(&NewInput->MouseX, &NewInput->MouseY);

	for (uint32 Index = 0; Index < sizeof(NewInput->Buttons)/sizeof(game_button_state); Index++) {
		NewInput->Buttons[Index].Changed = (OldInput->Buttons[Index].EndedDown ==
			NewInput->Buttons[Index].EndedDown) ? false : true;
	}
	return true;
}

internal void
CleanUp(platform_state *Platform)
{
	SDL_DestroyWindow(Platform->Window);
	Platform->Window = nullptr;

	SDL_DestroyRenderer(Platform->Renderer);
	Platform->Renderer = nullptr;
	SDL_Quit();
}

int main(int Count, char *Arguments[])
{
	platform_state Platform = {};
	assert(InitPlatform(&Platform));

	game_memory GameMemory = {};
	AllocateGameMemory(&GameMemory);
	assert(GameMemory.BaseAddress);

	playback_buffer PlaybackBuffer = NewPlaybackBuffer(32, 16, SafeTruncateUint64(GameMemory.Size));
	printf("Memory BaseAddress: %lu; GameMemory.Size: %lu MB", (uint64)GameMemory.BaseAddress, GameMemory.Size / (uint64)1e6);

	game_input OldInput = {};
	game_input NewInput = {};

	while (Platform.Running)
	{
		Platform.FPS.start();

		Platform.Running = ProcessInput(&OldInput, &NewInput, &Platform.Event);

		if (NewInput.p.EndedDown && NewInput.p.Changed) {
			Platform.PlaybackStarted = !Platform.PlaybackStarted;
			if (!Platform.PlaybackStarted) {
				ClearPlaybackBuffer(&PlaybackBuffer);
				for(int ButtonIndex = 0; ButtonIndex < 9; ButtonIndex++){
					NewInput.Buttons[ButtonIndex].EndedDown = false;
					NewInput.Buttons[ButtonIndex].Changed = false;
				}
			}
		}

		if (Platform.PlaybackStarted) {
			PeekAndStepPlaybackBuffer(&PlaybackBuffer, &NewInput, GameMemory.BaseAddress, Platform.FrameCount);
		}
		else {
			PushPlaybackBuffer(&PlaybackBuffer, &NewInput, GameMemory.BaseAddress, Platform.FrameCount);
		}

		UpdateAndRender(&GameMemory, &Platform, &NewInput);

		SDL_Delay(FRAME_DURATION - ((Platform.FPS.get_time() <= FRAME_DURATION) ? Platform.FPS.get_time() : FRAME_DURATION));
		Platform.FPS.update_avg_fps();

		//printf("fps: %f,\n", Platform.FPS.get_average_fps());
#if DEBUG_PROFILING
		for(uint32 i = 0; i < DEBUG_Last; i++){
			printf("%35s:%15lucy,%10lucy/op,%10.2f\n", DEBUG_TABLE_NAMES[i], DEBUG_CYCLE_TABLE[i].CycleCount, DEBUG_CYCLE_TABLE[i].CycleCount/DEBUG_CYCLE_TABLE[i].Calls, 100.0 * (real64)DEBUG_CYCLE_TABLE[i].CycleCount /(real64)DEBUG_CYCLE_TABLE[0].CycleCount);
		}
#endif
		//printf("\n");

		OldInput = NewInput;
		Platform.FrameCount++;
	}

	DestroyPlaybackBuffer(&PlaybackBuffer);
	CleanUp(&Platform);
	return 0;
}
