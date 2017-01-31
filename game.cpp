#if !defined(GAME_CPP)
#define GAME_CPP

#include "assert.h"
#include <string.h>
#include <stdio.h>

#include "constants_game.h"
#include "vec.h"
#include <stdint.h>
#include "boundry.h"
#include "game.h"
#include "rendering.h"
#include "rendering.cpp"
#include "misc.cpp"
#include "grid_rect.cpp"
#include "level_editing.cpp"
#include "draw_game.cpp"
#include "logic.cpp"

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	game_state *GameState = (game_state*)Memory->BaseAddress;
	if(GameState->MagicChecksum != 11789){
		GameState->MagicChecksum = 11789;
		assert(GameState);
		assert(sizeof(GameState) <= Memory->Size); 
		GameState->Mode = Game_Mode_Overworld;
		GameState->ActiveLayerIndex = 1;
		GameState->ActiveBrush = 1;
		GameState->SpriteAtlasActive = true;
		GameState->BitmapCount = 0;
		GameState->CurrentBitmapIndex = 0;
		loaded_bitmap SpriteSheet0 = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/tile_sheet.bmp");//(NOTE): Add success check
		AddBitmap(GameState, SpriteSheet0);
		loaded_bitmap SpriteSheet1 = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/tile_sheet1.bmp");//(NOTE): Add success check
		AddBitmap(GameState, SpriteSheet1);

		debug_read_file_result OverworldHandle = Memory->PlatformServices.ReadEntireFile((char*)"overworld");
		if(OverworldHandle .ContentsSize){
			memcpy(&GameState->Overworld, OverworldHandle.Contents, OverworldHandle.ContentsSize);
			Memory->PlatformServices.FreeFileMemory(OverworldHandle.Contents);
		}
		printf("Reading Overworld, success: %d\n", (OverworldHandle.Contents != 0));
	}

	//State Machine
	if(GameState->Mode != Game_Mode_Overworld){
		bool32 GameModeChanged = false;
		if(Input->e.EndedDown && Input->e.Changed){
			GameState->Mode = (GameState->Mode == Game_Mode_Edit) ? Game_Mode_Play : Game_Mode_Edit;
			GameModeChanged  = true;
		}else if(Input->t.EndedDown && Input->t.Changed){
			GameState->Mode = (GameState->Mode == Game_Mode_Tile) ? Game_Mode_Play : Game_Mode_Tile;
			GameModeChanged = true;
		}else if(Input->o.EndedDown && Input->o.Changed ){
			GameState->Mode = Game_Mode_Overworld;
		}
		if(GameModeChanged){
			ReloadLevel(GameState, Memory->PlatformServices);
		}
		if(Input->LeftCtrl.EndedDown && Input->s.EndedDown && Input->s.Changed){
			if(GameState->Mode == Game_Mode_Tile){
				GameState->Level.ForegroundLayerIndex = GameState->ActiveLayerIndex;
			}
			bool32 LevelWriteSuccess = Memory->PlatformServices.WriteEntireFile(GameState->Overworld.ActiveLevelName, sizeof(level), &GameState->Level);
			printf("Writing Level: \"%s\", success: %d;\n", GameState->Overworld.ActiveLevelName, LevelWriteSuccess);

			if(!GameState->Overworld.LevelInfos[GameState->LevelIndex].Exists){
				GameState->Overworld.LevelInfos[GameState->LevelIndex].Exists = true;
				bool32 WriteSuccess = Memory->PlatformServices.WriteEntireFile((char*)&"overworld", sizeof(overworld), &GameState->Overworld);
				printf("Writing Overworld, success: %d\n", WriteSuccess );
			}
		}
	}else{
		if(Input->LeftCtrl.EndedDown && Input->s.EndedDown && Input->s.Changed){
			if(!GameState->Overworld.LevelInfos[GameState->LevelIndex].Exists){
				bool32 WriteSuccess = Memory->PlatformServices.WriteEntireFile((char*)&"overworld", sizeof(overworld), &GameState->Overworld);
				printf("Writing Overworld, success: %d\n", WriteSuccess );
			}
		}
	}

	rectangle ScreenOutline = {0, 0, OffscreenBuffer.Width, OffscreenBuffer.Height};
	rectangle GameBoardRect = {0, OffscreenBuffer.Height - LEVEL_MAX_HEIGHT*DEST_TILE_SIZE_IN_PIXELS, LEVEL_MAX_WIDTH*DEST_TILE_SIZE_IN_PIXELS, OffscreenBuffer.Height};
	GameState->SpriteAtlasDestRect = {0, OffscreenBuffer.Height - GameState->Bitmaps[GameState->CurrentBitmapIndex].Height, GameState->Bitmaps[GameState->CurrentBitmapIndex].Width, OffscreenBuffer.Height};

	//Update and Render
	ClearOffscreenBuffer(OffscreenBuffer, color{100, 200, 255, 255});
	switch(GameState->Mode){
		case Game_Mode_Overworld:
			UpdateOverworld(GameState, ScreenOutline, GameBoardRect, Memory->PlatformServices, Input);
			DrawOverworld(&GameState->Overworld, OffscreenBuffer, ScreenOutline, GameBoardRect, Input->MouseX, Input->MouseY);
		break;
		case Game_Mode_Play:
			UpdateLogic(GameState, Input, Memory->PlatformServices);
			DrawEditModeLevelWithoutSolidAndSpikeBlocks(GameState, OffscreenBuffer, ScreenOutline);
			DrawLevelTilesInLayerRange(GameState, OffscreenBuffer, ScreenOutline, 0, GameState->Level.ForegroundLayerIndex);
			DrawSnakes(GameState, OffscreenBuffer, ScreenOutline);
			DrawLevelTilesInLayerRange(GameState, OffscreenBuffer, ScreenOutline, GameState->Level.ForegroundLayerIndex, LEVEL_MAX_LAYER_COUNT);
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
