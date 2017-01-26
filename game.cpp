#if !defined(GAME_CPP)
#define GAME_CPP

#include "rendering.h"
#include "game.h"
#include "rendering.cpp"
#include "misc.cpp"
#include "grid_rect.cpp"
#include "level_editing.cpp"
#include "draw_game.cpp"
#include "logic.cpp"

internal void
InitGameState(game_state *GameState, void *MemoryEnd)
{
	assert(GameState);
	char *BaseAddress = ((char*)GameState); 
	assert((BaseAddress + sizeof(GameState)) <= (char*)MemoryEnd); 
	GameState->Mode = Game_Mode_Play;
	GameState->ActiveLayerIndex = 1;
	GameState->ActiveBrush = 1;
	GameState->LevelCount = 1;
	GameState->SpriteAtlasActive = true;
	GameState->CurrentLevelName[0] = '0';
	GameState->CurrentLevelName[1] = '0';
	GameState->SpriteAtlas = DEBUGPlatformLoadBitmapFromFile((char*)"data/tile_sheet.bmp");//(NOTE): Add success check
}

internal void
UpdateAndRender(game_memory *Memory, offscreen_buffer OffscreenBuffer, game_input *Input)
{
	game_state *GameState = (game_state*)Memory->BaseAddress;
	if(GameState->MagicChecksum != 11789){
		GameState->MagicChecksum = 11789;
		InitGameState(GameState, (char*)Memory->BaseAddress + Memory->Size);
		ReloadLevel(GameState);
	}

	//State Machine
	bool32 GameStateChanged = false;
	if(Input->e.EndedDown && Input->e.Changed){
		GameState->Mode = (GameState->Mode == Game_Mode_Edit) ? Game_Mode_Play : Game_Mode_Edit;
		GameStateChanged = true;
	}else if(Input->t.EndedDown && Input->t.Changed){
		GameState->Mode = (GameState->Mode == Game_Mode_Tile) ? Game_Mode_Play : Game_Mode_Tile;
		GameStateChanged = true;
	}
	if(GameStateChanged){
		if(GameState->Mode != Game_Mode_Play){
			debug_read_file_result LevelHandle = DEBUGPlatformReadEntireFile(&GameState->CurrentLevelName[0]);
			if(LevelHandle.ContentsSize){
				memcpy(&GameState->Level, LevelHandle.Contents, LevelHandle.ContentsSize);
				DEBUGPlatformFreeFileMemory(LevelHandle.Contents);
			}
		}else{
			ReloadLevel(GameState);
		}
	}
	if(Input->LeftCtrl.EndedDown && Input->s.EndedDown && Input->s.Changed){
		if(GameState->Mode == Game_Mode_Tile){
			GameState->Level.ForegroundLayerIndex = GameState->ActiveLayerIndex;
		}
		bool32 WriteSuccess = DEBUGPLatformWriteEntireFile(&GameState->CurrentLevelName[0], sizeof(level), &GameState->Level);
		printf("Save Initiated, (1-success, 0-fail): %d  to %s\n", WriteSuccess, GameState->CurrentLevelName);
	}

	rectangle ScreenOutline = {0, 0, OffscreenBuffer.Width, OffscreenBuffer.Height};
	rectangle GameBoardRect = {0, OffscreenBuffer.Height - LEVEL_MAX_HEIGHT*BLOCK_HEIGHT_IN_PIXELS, LEVEL_MAX_WIDTH*BLOCK_WIDTH_IN_PIXELS, OffscreenBuffer.Height};
	GameState->SpriteAtlasRect = {0, OffscreenBuffer.Height - GameState->SpriteAtlas.Height, GameState->SpriteAtlas.Width, OffscreenBuffer.Height};

	//Update and Render
	ClearOffscreenBuffer(OffscreenBuffer, color{100, 200, 255, 255});
	switch(GameState->Mode){
		case Game_Mode_Play:
			UpdateLogic(GameState, Input);
			DrawPlayModeElements(GameState, OffscreenBuffer, ScreenOutline);
			DrawPlayModeLevel(GameState, OffscreenBuffer, ScreenOutline, 0, GameState->Level.ForegroundLayerIndex);
			DrawSnakes(GameState, OffscreenBuffer, ScreenOutline);
			DrawPlayModeLevel(GameState, OffscreenBuffer, ScreenOutline, GameState->Level.ForegroundLayerIndex, LEVEL_MAX_LAYER_COUNT);
		break;
		case Game_Mode_Edit:
			EditLevel(&GameState->Level, ScreenOutline, &GameState->ActiveBrush, Input);
			DrawEditModeLevel(GameState, OffscreenBuffer, ScreenOutline);
			DrawSnakes(GameState, OffscreenBuffer, ScreenOutline);
			DrawEditBrush(GameState, OffscreenBuffer, ScreenOutline, Input->MouseX, Input->MouseY);
		break;
		case Game_Mode_Tile:
			TileLevel(GameState, ScreenOutline, GameBoardRect, Input);
			DrawTileModeLevelAndUI(GameState, OffscreenBuffer, ScreenOutline);
			DrawTileBrush(GameState, OffscreenBuffer, ScreenOutline, Input->MouseX, Input->MouseY);
		break;
	}
	DrawRectOutline(OffscreenBuffer, GameBoardRect, color{255, 255, 255, 255});
}

#endif //GAME_CPP
