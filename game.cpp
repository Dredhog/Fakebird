#if !defined(GAME_CPP)
#define GAME_CPP

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "constants_game.h"
#include "boundry.h"
#include "vector.h"
#include "matrix.h"
#include "matrix.cpp"
#include "game.h"
#include "util.cpp"
#include "grids_transformations.cpp"
#include "misc.cpp"
#include "rendering.h"
#include "quad_rendering.cpp"
#include "triangle_rendering.cpp"
#include "level_editing.cpp"
#include "draw_game.cpp"
#include "logic.cpp"

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	INIT_DEBUGING();
	BEGIN_TIMED_BLOCK(GameUpdateAndRender);
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
		rectangle DestRect = {0, (real32)(OffscreenBuffer.Height - Bitmap.Height), (real32)Bitmap.Width, (real32)OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, 77, 77);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/cloud.bmp");//(NOTE): Add success check
		DestRect = {0, (real32)(OffscreenBuffer.Height - Bitmap.Height), (real32)Bitmap.Width, (real32)OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/dark_cloud.bmp");//(NOTE): Add success check
		DestRect = {0, (real32)(OffscreenBuffer.Height - Bitmap.Height), (real32)Bitmap.Width, (real32)OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/mountain.bmp");//(NOTE): Add success check
		DestRect = {0, (real32)(OffscreenBuffer.Height - Bitmap.Height), (real32)Bitmap.Width, (real32)OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/purple_cloud.bmp");//(NOTE): Add success check
		DestRect = {0, (real32)(OffscreenBuffer.Height - Bitmap.Height), (real32)Bitmap.Width, (real32)OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/crate.bmp");//(NOTE): Add success check
		DestRect = {0, (real32)(OffscreenBuffer.Height - Bitmap.Height), (real32)Bitmap.Width, (real32)OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		Bitmap = Memory->PlatformServices.LoadBitmapFromFile((char*)"data/grid.bmp");//(NOTE): Add success check
		DestRect = {0, (real32)(OffscreenBuffer.Height - Bitmap.Height), (real32)Bitmap.Width, (real32)OffscreenBuffer.Height};
		AddTilemap(Palette, Bitmap, DestRect, Bitmap.Width, Bitmap.Height);

		debug_read_file_result OverworldHandle = Memory->PlatformServices.ReadEntireFile((char*)"overworld");
		if(OverworldHandle .ContentsSize){
			memcpy(&GameState->Overworld, OverworldHandle.Contents, OverworldHandle.ContentsSize);
			Memory->PlatformServices.FreeFileMemory(OverworldHandle);
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
			CLEAR_DEBUG_CYCLE_TABLE();
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

	rectangle ScreenOutline = {0, 0, (real32)OffscreenBuffer.Width, (real32)OffscreenBuffer.Height};
	rectangle GameBoardRect = {0, (real32)(OffscreenBuffer.Height - LEVEL_MAX_HEIGHT*DEST_TILE_SIZE_IN_PIXELS), (real32)(LEVEL_MAX_WIDTH*DEST_TILE_SIZE_IN_PIXELS), (real32)OffscreenBuffer.Height};
	UpdateTilemapRects(&GameState->TilemapPalette, OffscreenBuffer.Height);

	{
		projection *Projection = &GameState->Projection;
		Projection->CameraP = {0, 0, 1.f};
		Projection->CameraDir = {0, 0, -1.0f};
		Projection->UnitInPixels = (real32)DEST_TILE_SIZE_IN_PIXELS;
		Projection->FocalLength = 1;
		Projection->GridWidth = GameState->Level.Width;
		Projection->GridHeight = GameState->Level.Height;
		Projection->ScreenWidthInPixels = OffscreenBuffer.Width;
		Projection->ScreenHeightInPixels = OffscreenBuffer.Height;
	}
	GameState->Projection.MeshAngle += 1;

	//Update and Render
	ClearOffscreenBuffer(OffscreenBuffer, color{160, 200, 255, 255});
#define RENDER_TEXTURED_CUBE 0
#if RENDER_TEXTURED_CUBE != 0
	{
		triangle_mesh Mesh = {};
		Mesh.VerticeCount = 16;
		//front face
		Mesh.Vertices[0].P = { 1, -1,  1};
		Mesh.Vertices[1].P = { 1,  1,  1};
		Mesh.Vertices[2].P = {-1,  1,  1};
		Mesh.Vertices[3].P = {-1, -1,  1};
		Mesh.Vertices[0].UV = {1, 1};
		Mesh.Vertices[1].UV = {1, 0};
		Mesh.Vertices[2].UV = {0, 0};
		Mesh.Vertices[3].UV = {0, 1};

		//back face
		Mesh.Vertices[4].P = {-1, -1, -1};
		Mesh.Vertices[5].P = {-1,  1, -1};
		Mesh.Vertices[6].P = { 1,  1, -1};
		Mesh.Vertices[7].P = { 1, -1, -1};
		Mesh.Vertices[4].UV = {1, 1};
		Mesh.Vertices[5].UV = {1, 0};
		Mesh.Vertices[6].UV = {0, 0};
		Mesh.Vertices[7].UV = {0, 1};

		//top face
		Mesh.Vertices[8].P   = { 1,  1,  1};
		Mesh.Vertices[9].P   = { 1,  1, -1};
		Mesh.Vertices[10].P  = {-1,  1, -1};
		Mesh.Vertices[11].P  = {-1,  1,  1};
		Mesh.Vertices[8].UV  = {1, 1};
		Mesh.Vertices[9].UV  = {1, 0};
		Mesh.Vertices[10].UV = {0, 0};
		Mesh.Vertices[11].UV = {0, 1};

		//bottom face
		Mesh.Vertices[12].P   = {-1, -1,  1};
		Mesh.Vertices[13].P   = {-1, -1, -1};
		Mesh.Vertices[14].P  = { 1, -1, -1};
		Mesh.Vertices[15].P  = { 1, -1,  1};
		Mesh.Vertices[12].UV  = {1, 1};
		Mesh.Vertices[13].UV  = {1, 0};
		Mesh.Vertices[14].UV = {0, 0};
		Mesh.Vertices[15].UV = {0, 1};

		//Sides
		Mesh.IndiceCount = 36;
		Mesh.Indices[0] = 0;
		Mesh.Indices[1] = 1;
		Mesh.Indices[2] = 2;
		Mesh.Indices[3] = 0;
		Mesh.Indices[4] = 2;
		Mesh.Indices[5] = 3;

		Mesh.Indices[6] = 4;
		Mesh.Indices[7] = 5;
		Mesh.Indices[8] = 6;
		Mesh.Indices[9] = 4;
		Mesh.Indices[10] = 6;
		Mesh.Indices[11] = 7;

		Mesh.Indices[12] = 1;
		Mesh.Indices[13] = 0;
		Mesh.Indices[14] = 7;
		Mesh.Indices[15] = 1;
		Mesh.Indices[16] = 7;
		Mesh.Indices[17] = 6;

		Mesh.Indices[18] = 3;
		Mesh.Indices[19] = 2;
		Mesh.Indices[20] = 5;
		Mesh.Indices[21] = 3;
		Mesh.Indices[22] = 5;
		Mesh.Indices[23] = 4;

		//top and bottom
		Mesh.Indices[24] = 8;
		Mesh.Indices[25] = 9;
		Mesh.Indices[26] = 10;
		Mesh.Indices[27] = 8;
		Mesh.Indices[28] = 10;
		Mesh.Indices[29] = 11;

		Mesh.Indices[30] = 12;
		Mesh.Indices[31] = 13;
		Mesh.Indices[32] = 14;
		Mesh.Indices[33] = 12;
		Mesh.Indices[34] = 14;
		Mesh.Indices[35] = 15;
		mat4 CameraTransform = Mat4Camera(GameState->Projection.CameraP, GameState->Projection.CameraDir);
		mat4 Tranlation = Mat4Translate(sinf(0.1f*3.14159f/180.0f*GameState->Projection.MeshAngle), 0, -2.0f);
		mat4 RotX = Mat4RotateX(GameState->Projection.MeshAngle);
		mat4 RotY = Mat4RotateY(20.0f*sinf(0.05f*GameState->Projection.MeshAngle));
		mat4 RotZ = Mat4RotateZ(0.0f);
		mat4 Rotation = MulMat4(RotZ, MulMat4(RotY, RotX));

		mat4 FinalTransform = MulMat4(CameraTransform, MulMat4(Tranlation, Rotation));
		//PrintMat4(FinalTransform);

		DrawTriangleMesh(OffscreenBuffer, GameState->TilemapPalette.Tilemaps[5].Bitmap, &Mesh, FinalTransform );
	}
#endif
#undef RENDER_TEXTURED_CUBE 
	switch(GameState->Mode){
		case Game_Mode_Overworld:
			UpdateOverworld(GameState, ScreenOutline, GameBoardRect, Memory->PlatformServices, Input);
			DrawOverworld(&GameState->Overworld, OffscreenBuffer, ScreenOutline, GameBoardRect, Input->MouseX, Input->MouseY);
		break;
		case Game_Mode_Play:
		{
		BEGIN_TIMED_BLOCK(UpdateLogic);
			UpdateLogic(GameState, Input, Memory->PlatformServices);
			UpdateAnimatedTiles(&GameState->Level);
		END_TIMED_BLOCK(UpdateLogic);
			DrawAnimatedTiles(&GameState->Level, OffscreenBuffer, &GameState->Projection, Input, 0);
			DrawEditModeLevelWithoutSolidAndSpikeBlocks(GameState, OffscreenBuffer, ScreenOutline);
			DrawSnakesWithPerspective(GameState, OffscreenBuffer, ScreenOutline);
			DrawLevelTilesInLayerRange(&GameState->Level, OffscreenBuffer, &GameState->Projection, 0, LEVEL_MAX_LAYER_COUNT);
			DrawAnimatedTiles(&GameState->Level, OffscreenBuffer, &GameState->Projection, Input, 1);
		}
		break;
		case Game_Mode_Edit:
			EditLevel(&GameState->Level, ScreenOutline, &GameState->EditBrush, Input);
			DrawEditModeLevel(GameState, OffscreenBuffer, ScreenOutline);
			DrawSnakesInEditMode(GameState, OffscreenBuffer, ScreenOutline);
			DrawEditBrush(GameState, OffscreenBuffer, ScreenOutline, Input->MouseX, Input->MouseY);
		break;
		case Game_Mode_Tile:
			TileLevel(&GameState->Level, &GameState->TilemapPalette, &GameState->TileBrush, ScreenOutline, GameBoardRect, Input, &GameState->Projection);
			DrawAnimatedTiles(&GameState->Level, OffscreenBuffer, &GameState->Projection, Input, 0);
			DrawTileModeLevelAndUI(GameState, OffscreenBuffer, ScreenOutline);
			DrawAnimatedTiles(&GameState->Level, OffscreenBuffer, &GameState->Projection, Input, 1);
			DrawTileBrush(&GameState->TileBrush, OffscreenBuffer, ScreenOutline, Input->MouseX, Input->MouseY, &GameState->Projection);
		break;
	}
	DrawRectOutline(OffscreenBuffer, GameBoardRect, color{255, 255, 255, 255});

	END_TIMED_BLOCK(GameUpdateAndRender);
}

#endif //GAME_CPP
