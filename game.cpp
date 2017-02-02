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
#include "grids_transformations.cpp"
#include "level_editing.cpp"
#include "draw_game.cpp"
#include "logic.cpp"
#include "math.h"

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	game_state *GameState = (game_state*)Memory->BaseAddress;
	if(GameState->MagicChecksum != 11789){
		GameState->MagicChecksum = 11789;
		assert(GameState);
		assert(sizeof(GameState) <= Memory->Size); 
		GameState->Mode = Game_Mode_Overworld;
		GameState->TileBrush.ActiveLayerIndex = 1;
		GameState->EditBrush = Tile_Type_Solid;
		
		tilemap_palette *Palette = &GameState->TilemapPalette;
		Palette->Active = true;
		Palette->TilemapCapacity = BITMAP_MAX_COUNT;

		loaded_bitmap Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/tile_sheet.bmp");//(NOTE): Add success check
		rectangle DestRect = {0, OffscreenBuffer.Height - Bitmap.Height, Bitmap.Width, OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, 77, 77);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/cloud.bmp");//(NOTE): Add success check
		DestRect = {0, OffscreenBuffer.Height - Bitmap.Height, Bitmap.Width, OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/dark_cloud.bmp");//(NOTE): Add success check
		DestRect = {0, OffscreenBuffer.Height - Bitmap.Height, Bitmap.Width, OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/mountain.bmp");//(NOTE): Add success check
		DestRect = {0, OffscreenBuffer.Height - Bitmap.Height, Bitmap.Width, OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

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
				GameState->Level.ForegroundLayerIndex = GameState->TileBrush.ActiveLayerIndex;
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
	UpdateTilemapRects(&GameState->TilemapPalette, OffscreenBuffer.Height);

#if 1
	{
		projection Projection = {}; 
		Projection.CameraP = vec3f{0, 0, 1.0f};
		Projection.UnitInPixels = (real32)DEST_TILE_SIZE_IN_PIXELS;
		Projection.FocalLength = 1;
		Projection.GridWidth = GameState->Level.Width;
		Projection.GridHeight = GameState->Level.Height;
		Projection.ScreenWidthInPixels = OffscreenBuffer.Width;
		Projection.ScreenHeightInPixels = OffscreenBuffer.Height;
		GameState->Projection = Projection;
	}
#else
	GameState->Projection.CameraP.Z += 0.001f;
#endif

	//Update and Render
	ClearOffscreenBuffer(OffscreenBuffer, color{100, 200, 255, 255});
	switch(GameState->Mode){
		case Game_Mode_Overworld:
			UpdateOverworld(GameState, ScreenOutline, GameBoardRect, Memory->PlatformServices, Input);
			DrawOverworld(&GameState->Overworld, OffscreenBuffer, ScreenOutline, GameBoardRect, Input->MouseX, Input->MouseY);
		break;
		case Game_Mode_Play:
			UpdateLogic(GameState, Input, Memory->PlatformServices);
			UpdateAnimatedTiles(&GameState->Level);
			DrawAnimatedTiles(&GameState->Level, OffscreenBuffer, &GameState->Projection);
			DrawEditModeLevelWithoutSolidAndSpikeBlocks(GameState, OffscreenBuffer, ScreenOutline);
			DrawSnakesWithPerspective(GameState, OffscreenBuffer, ScreenOutline);
			DrawLevelTilesInLayerRange(&GameState->Level, OffscreenBuffer, &GameState->Projection, 0, LEVEL_MAX_LAYER_COUNT);
		break;
		case Game_Mode_Edit:
			EditLevel(&GameState->Level, ScreenOutline, &GameState->EditBrush, Input);
			DrawEditModeLevel(GameState, OffscreenBuffer, ScreenOutline);
			DrawSnakesInEditMode(GameState, OffscreenBuffer, ScreenOutline);
			DrawEditBrush(GameState, OffscreenBuffer, ScreenOutline, Input->MouseX, Input->MouseY);
		break;
		case Game_Mode_Tile:
			TileLevel(&GameState->Level, &GameState->TilemapPalette, &GameState->TileBrush, ScreenOutline, GameBoardRect, Input, &GameState->Projection);
			DrawAnimatedTiles(&GameState->Level, OffscreenBuffer, &GameState->Projection);
			DrawTileModeLevelAndUI(GameState, OffscreenBuffer, ScreenOutline);
			DrawTileBrush(&GameState->TileBrush, OffscreenBuffer, ScreenOutline, Input->MouseX, Input->MouseY, &GameState->Projection);
		break;
	}
	DrawRectOutline(OffscreenBuffer, GameBoardRect, color{255, 255, 255, 255});
}

#endif //GAME_CPP
