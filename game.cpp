#if !defined(GAME_CPP)
#define GAME_CPP

/*
#include "platform.h"
#include "game.h"
#include "globals.h"
#include "vec2.h"
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <x86intrin.h>
*/

internal inline vec2i
GetGridP(int32 X, int32 Y, SDL_Rect ScreenRect){
	return vec2i{(X-ScreenRect.x)/BLOCK_WIDTH_IN_PIXELS, (ScreenRect.h - (Y-ScreenRect.y))/BLOCK_HEIGHT_IN_PIXELS};
}

internal inline SDL_Rect
GetGridRect(int32 X, int32 Y, SDL_Rect ScreenRect){
	return SDL_Rect{X*BLOCK_WIDTH_IN_PIXELS,
					ScreenRect.h- (Y+1)*BLOCK_HEIGHT_IN_PIXELS,
					BLOCK_WIDTH_IN_PIXELS,
					BLOCK_HEIGHT_IN_PIXELS};
}

internal inline SDL_Rect
GetGridRectFromScreenCoords(int32 X, int32 Y, SDL_Rect ScreenRect){
	vec2i  GridP = GetGridP(X, Y, ScreenRect);
	return GetGridRect(GridP.X, GridP.Y, ScreenRect);
}


internal inline vec2i
GetTileGridP(int32 X, int32 Y, SDL_Rect TileWindowRect){
	return vec2i{(X-TileWindowRect.x)/SOURCE_BLOCK_WIDTH_IN_PIXELS, (Y-TileWindowRect.y)/SOURCE_BLOCK_HEIGHT_IN_PIXELS};
}

internal inline SDL_Rect
GetTileGridRect(int32 X, int32 Y, SDL_Rect TileWindowRect){
	vec2i GridP = GetTileGridP(X, Y, TileWindowRect);
	return SDL_Rect{GridP.X*SOURCE_BLOCK_WIDTH_IN_PIXELS,
					GridP.Y*SOURCE_BLOCK_WIDTH_IN_PIXELS,
					SOURCE_BLOCK_WIDTH_IN_PIXELS,
					SOURCE_BLOCK_HEIGHT_IN_PIXELS};
}

internal void
DrawPlayModeElements(game_state *GameState, SDL_Renderer *Renderer, SDL_Rect ScreenOutline){
	for(uint32 j = 0; j < GameState->Level.Width; j++){
		for(uint32 k = 0; k < GameState->Level.Height; k++){
			SDL_Rect DestRect = GetGridRect(j, k, ScreenOutline);

			uint32 Value = GameState->Level.Occupancy[j][k];
			if(Value <= 4 && Value > 1 && Value != 3){
				color TileColor;
				if(Value == 4){
				 	TileColor = (GameState->FruitRemaining == 0) ? TILE_COLORS[Tile_Type_Goal] : TILE_COLORS[Tile_Type_Goal-1];
				}else{
					TileColor = TILE_COLORS[Value - 1];
				}
				SDL_SetRenderDrawColor(Renderer, TileColor.R, TileColor.G, TileColor.B, TileColor.A);
				SDL_RenderFillRect(Renderer, &DestRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 100);
				SDL_RenderDrawRect(Renderer, &DestRect);
			}
		}
	}
}

internal void
DrawPlayModeLevel(game_state *GameState, SDL_Renderer *Renderer, SDL_Rect ScreenOutline, int32 StartLayer, int32 EndLayer){
	for(int32 i = StartLayer; i < EndLayer; i++){
		for(uint32 j = 0; j < GameState->Level.Width; j++){
			for(uint32 k = 0; k < GameState->Level.Height; k++){
				SDL_Rect DestRect = GetGridRect(j, k, ScreenOutline);
				SDL_RenderCopy(Renderer, GameState->TileTexture, &GameState->Level.Tiles[i][j][k].Source, &DestRect);
			}
		}
	}
}

internal void
DrawEditModeLevel(game_state *GameState, SDL_Renderer *Renderer, SDL_Rect ScreenOutline){
	for(uint32 j = 0; j < GameState->Level.Width; j++){
		for(uint32 k = 0; k < GameState->Level.Height; k++){
			SDL_Rect DestRect = GetGridRect(j, k, ScreenOutline);

			uint32 Value = GameState->Level.Occupancy[j][k];
			if(Value <= 4 && Value > 0){
				color TileColor = TILE_COLORS[Value - 1];
				if(Value == 4){
					 TileColor = (GameState->FruitRemaining == 0) ? TILE_COLORS[Tile_Type_Goal] : TILE_COLORS[Tile_Type_Goal-1];
				}
				SDL_SetRenderDrawColor(Renderer, TileColor.R, TileColor.G, TileColor.B, TileColor.A);
				SDL_RenderFillRect(Renderer, &DestRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 100);
				SDL_RenderDrawRect(Renderer, &DestRect);
			}
		}
	}
}

internal uint32
CapInt32(int32 a, int32 t, int32 b){
	if(t < a){
		return a;
	}else if (t > b){
		return b;
	}
	return t;
}

internal void
DrawTileModeLevelAndUI(game_state *GameState, SDL_Renderer *Renderer, SDL_Rect ScreenOutline){
	for(uint32 j = 0; j < GameState->Level.Width; j++){
		for(uint32 k = 0; k < GameState->Level.Height; k++){
			SDL_Rect DestRect = GetGridRect(j, k, ScreenOutline);

			uint32 Value = GameState->Level.Occupancy[j][k];
			if(Value <= 4 && Value > 0){
				color TileColor = TILE_COLORS[Value - 1];
				SDL_SetRenderDrawColor(Renderer, TileColor.R, TileColor.G, TileColor.B, TileColor.A);
				SDL_RenderFillRect(Renderer, &DestRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 100);
				SDL_RenderDrawRect(Renderer, &DestRect);
			}
		}
	}
	for(int32 i = 0; i <= GameState->ActiveLayerIndex; i++){
		for(uint32 j = 0; j < GameState->Level.Width; j++){
			for(uint32 k = 0; k < GameState->Level.Height; k++){
				SDL_Rect DestRect = GetGridRect(j, k, ScreenOutline);
				SDL_RenderCopy(Renderer, GameState->TileTexture, &GameState->Level.Tiles[i][j][k].Source, &DestRect);
			}
		}
	}
	if(GameState->TileWindowActive){
		color Color = TILEMAP_BACKGROUND_COLORS[GameState->ActiveLayerIndex];
		SDL_SetRenderDrawColor(Renderer, Color.R, Color.B, Color.B, 100);
		SDL_RenderFillRect(Renderer, &GameState->TileWindowRect);
		SDL_RenderCopy(Renderer, GameState->TileTexture, 0, &GameState->TileWindowRect);
	}
}

internal void
DrawEditBrush(game_state *GameState, SDL_Renderer *Renderer, SDL_Rect ScreenOutline, int32 MouseX, int32 MouseY){
	SDL_Rect MouseRect = GetGridRectFromScreenCoords(MouseX, MouseY, ScreenOutline);
	if(GameState->ActiveBrush <= 4 && GameState->ActiveBrush > 0){
		color TileColor = TILE_COLORS[GameState->ActiveBrush - 1];
		SDL_SetRenderDrawColor(Renderer, TileColor.R, TileColor.G, TileColor.B, TileColor.A);
		SDL_RenderFillRect(Renderer, &MouseRect);
		SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
		SDL_RenderDrawRect(Renderer, &MouseRect);
	}else if(GameState->ActiveBrush >= SNAKE_ID_OFFSET && GameState->ActiveBrush < SNAKE_ID_OFFSET + 3){
		color SnakeColor = SNAKE_COLORS[GameState->ActiveBrush-SNAKE_ID_OFFSET][0];
		SDL_SetRenderDrawColor(Renderer, SnakeColor.R, SnakeColor.G, SnakeColor.B, SnakeColor.A);
		SDL_RenderFillRect(Renderer, &MouseRect);
	}
}

internal void
DrawTileBrush(game_state *GameState, SDL_Renderer *Renderer, SDL_Rect ScreenOutline, int32 MouseX, int32 MouseY){
	SDL_Rect MouseGridRect = GetGridRectFromScreenCoords(MouseX, MouseY, ScreenOutline);
	SDL_RenderCopy(Renderer, GameState->TileTexture, &GameState->ActiveTileBrush.Source, &MouseGridRect);
}

internal void
DrawSnakes(game_state *GameState, SDL_Renderer *Renderer){
	for(uint32 SnakeIndex = 0; SnakeIndex  < GameState->Level.SnakeCount; SnakeIndex++){
		snake *Snake = &GameState->Level.Snakes[SnakeIndex];
		for(uint32 p = 0; p < Snake->Length; p++){
			vec2i PartPos =  Snake->Parts[p].GridP;
			SDL_Rect CurrentRect =  SDL_Rect{(int32)(PartPos.X*BLOCK_WIDTH_IN_PIXELS),
									(int32)(SCREEN_HEIGHT - (PartPos.Y+1)*BLOCK_HEIGHT_IN_PIXELS),
									(int32)BLOCK_WIDTH_IN_PIXELS,
									(int32)BLOCK_HEIGHT_IN_PIXELS};
			color Color = (p % 2 == 0) ? Snake->Color0 : Snake->Color1;
			SDL_SetRenderDrawColor(Renderer, Color.R, Color.G, Color.B, Color.A);
			SDL_RenderFillRect(Renderer, &CurrentRect);
		}
	}
}

internal void
AddSnakeToLevel(level *Level, snake Snake){
	assert(Level->SnakeCount >= 0 && Level->SnakeCount < SNAKE_MAX_COUNT);
	Snake.SnakeID = Level->SnakeCount + SNAKE_ID_OFFSET;
	Level->Snakes[Level->SnakeCount++] = Snake;
}

internal void
PlayNextSnake(game_state *GameState){
	uint64 Index = GameState->Player - &GameState->Level.Snakes[0];
	GameState->Player = &GameState->Level.Snakes[(Index+1) % GameState->Level.SnakeCount];
}

internal void
DeleteSnake(level *Level, uint32 SnakeID){
	uint32 RemoveIndex = SnakeID - SNAKE_ID_OFFSET;
	assert(RemoveIndex >= 0 && RemoveIndex < Level->SnakeCount);

	for(uint32 p = 0; p < Level->Snakes[RemoveIndex].Length; p++){
		vec2i PartP = Level->Snakes[RemoveIndex].Parts[p].GridP;
		Level->Occupancy[PartP.X][PartP.Y] = Tile_Type_Empty;
	}
	for(uint32 i = RemoveIndex; i < Level->SnakeCount-1; i++){
		Level->Snakes[i] = Level->Snakes[i+1];
	}
	Level->SnakeCount--;

	for(uint32 i = RemoveIndex; i < Level->SnakeCount; i++){
		Level->Snakes[i].SnakeID = i + SNAKE_ID_OFFSET;
		for(uint32 p = 0; p < Level->Snakes[i].Length; p++){
			vec2i PartP = Level->Snakes[i].Parts[p].GridP;
			Level->Occupancy[PartP.X][PartP.Y] = Level->Snakes[i].SnakeID;
		}
	}
}

internal void
InitGameState(game_state *GameState, void *MemoryEnd)
{
	assert(GameState);
	char *BaseAddress = ((char*)GameState); 
	assert((BaseAddress + sizeof(GameState)) <= (char*)MemoryEnd); 
	GameState->Mode = Game_Mode_Edit;
	GameState->ActiveLayerIndex = 0;
	GameState->ActiveBrush = 1;
	GameState->TileWindowActive = true;
}

internal void
LoadTileTextureAndAssignDimensions(game_state *GameState, SDL_Renderer *Renderer, char *FileName){
	SDL_Texture *TileTexture = DEBUGPlatformLoadImageFromFile(Renderer, FileName);
	if(TileTexture){
		GameState->TileTexture = TileTexture;
	}else{
		printf("Game: Spritesheet Load Error: Texture == NULL");
	}
	SDL_QueryTexture(GameState->TileTexture, 0, 0, &GameState->TextureWidth, &GameState->TextureHeight);
}

internal void
ClearVisitedSnakes(game_state *GameState){
	GameState->VisitedCount = 0;
	for(uint32 i = 0; i < GameState->Level.SnakeCount; i++){
		GameState->Visited[i] = false;
		GameState->VisitedSnakes[i] = 0;
	}
}

internal void
NewGame(game_state *GameState)
{
	debug_read_file_result LevelHandle = DEBUGPlatformReadEntireFile((char*)TempName);
	if(LevelHandle.ContentsSize){
		memcpy(&GameState->Level, LevelHandle.Contents, LevelHandle.ContentsSize);
		DEBUGPlatformFreeFileMemory(LevelHandle.Contents);
GameState->FruitRemaining = 0;
		for(uint32 i = 0; i < GameState->Level.Width; i++){
			for(uint32 j = 0; j < GameState->Level.Height; j++){
				uint32 Value = GameState->Level.Occupancy[i][j];
				if(Value == 2){
					GameState->FruitRemaining++;
				}else if(Value == 4){
					GameState->PortalP = vec2i{(int32)i, (int32)j};
				}
			}
		}

		if(GameState->Level.SnakeCount > 0){
			GameState->Player = &GameState->Level.Snakes[0];
		}else{
			GameState->Player = {};
		}
	}else{
		GameState->Level = {}; 
		GameState->Player = {};
		GameState->Level.Width = LEVEL_MAX_WIDTH;
		GameState->Level.Height = LEVEL_MAX_HEIGHT;
	}
	ClearVisitedSnakes(GameState);
}

internal inline bool32
IsInBounds(int32 i, int32 j, int32 MaxI, int32 MaxJ){
	if(i < 0 || j < 0 || i >= MaxI || j >= MaxJ){
		return false;
	}	
	return true;
}

internal bool32
IsRecursiveVisitPushable(game_state *GameState, uint32 SnakeID, uint32 FirstSnakeID, vec2i Dir){
	uint32 SnakeIndex = SnakeID-SNAKE_ID_OFFSET;
	assert(SnakeIndex >= 0 && SnakeIndex < GameState->Level.SnakeCount);
	snake *Snake = GameState->Level.Snakes + SnakeIndex;
	level *Level = &GameState->Level;

	GameState->Visited[SnakeIndex] = true;
	GameState->VisitedSnakes[GameState->VisitedCount++] = Snake;

	for(uint32 i = 0; i < Snake->Length; i++){
		vec2i DestP = Snake->Parts[i].GridP + Dir;
		if(!IsInBounds(DestP.X, DestP.Y, Level->Width, Level->Height)){
			return false;
		}

		uint32 DestValue = Level->Occupancy[DestP.X][DestP.Y];
		if(	DestValue == FirstSnakeID || DestValue == 1 || DestValue == 2 || (DestValue == 3 && Dir.Y >= 0)){
			return false;
		}else if( DestValue >= SNAKE_ID_OFFSET && !GameState->Visited[DestValue-SNAKE_ID_OFFSET] &&
					!IsRecursiveVisitPushable(GameState, DestValue, FirstSnakeID, Dir)){
			return false;
		}
	}
	return true;
}

internal void
PushVisitedSnakes(game_state *GameState, level *Level, vec2i Dir){
	for(uint32 s = 0; s < GameState->VisitedCount; s++){
		snake *Snake = GameState->VisitedSnakes[s];
		for(uint32 p = 0; p < Snake->Length; p++){
			vec2i PartPos = Snake->Parts[p].GridP;
			Level->Occupancy[PartPos.X][PartPos.Y] = Tile_Type_Empty;
		}
	}
	for(uint32 s = 0; s < GameState->VisitedCount; s++){
		snake *Snake = GameState->VisitedSnakes[s];
		for(uint32 p = 0; p < Snake->Length; p++){
			Snake->Parts[p].GridP += Dir;
			vec2i PartPos = Snake->Parts[p].GridP;
			Level->Occupancy[PartPos.X][PartPos.Y] = Snake->SnakeID;
		}
	}
}

internal bool
WillASnakeBePushedOnSpikes(game_state *GameState, vec2i Dir){
	for(uint32 s = 0; s < GameState->VisitedCount; s++){
		snake *Snake = GameState->VisitedSnakes[s];
		for(uint32 p = 0; p < Snake->Length; p++){
			vec2i DestDir = Snake->Parts[p].GridP + Dir;
			if(GameState->Level.Occupancy[DestDir.X][DestDir.Y] == Tile_Type_Spikes){
				return true;
			}
		}
	}
	return false;
}


internal void
UpdateLogic(game_state *GameState, game_input *Input){
	if(!GameState->Player){
		return;
	}
	if(Input->Space.EndedDown && Input->Space.Changed){
		PlayNextSnake(GameState);
	}
	if(Input->r.EndedDown && Input->r.Changed){
		NewGame(GameState);
	}

	snake *Player = GameState->Player;
	level *Level = &GameState->Level;
	snake_part *Head = Player->Parts;
	snake_part *Tail = Player->Parts + (Player->Length-1);

	vec2i Direction = {};
	if(Input->ArrowRight.EndedDown && Input->ArrowRight.Changed){
		Direction = {1, 0};
	}else if(Input->ArrowLeft.EndedDown && Input->ArrowLeft.Changed){
		Direction = {-1, 0};
	}else if(Input->ArrowUp.EndedDown && Input->ArrowUp.Changed){
		Direction = {0, 1};
	}else if(Input->ArrowDown.EndedDown && Input->ArrowDown.Changed){
		Direction = {0, -1};
	}
	
	vec2i NewPos = Head->GridP + Direction;
	if(IsInBounds(NewPos.X, NewPos.Y, Level->Width, Level->Height)){
		uint32 Value = Level->Occupancy[NewPos.X][NewPos.Y];
	
		ClearVisitedSnakes(GameState);

		bool32 move = false;
		if(Value == Tile_Type_Empty || Value == Tile_Type_Goal){
			Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = Tile_Type_Empty;
			move = true;
		}else if(Value >= SNAKE_ID_OFFSET && Value != Player->SnakeID && IsRecursiveVisitPushable(GameState, Value, Player->SnakeID, Direction)){
			PushVisitedSnakes(GameState, &GameState->Level, Direction);
			Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = Tile_Type_Empty;
			move = true;
		}else if(Value == Tile_Type_Fruit){
			Player->Length++;
			assert(Player->Length <= SNAKE_MAX_LENGTH);
			GameState->FruitRemaining--;
			move = true;
		}
		if(move){
			for(uint32 i = Player->Length-1; i > 0; i--){
				Player->Parts[i].GridP = Player->Parts[i-1].GridP;
			}
			Level->Occupancy[NewPos.X][NewPos.Y] = Player->SnakeID;
			Head->GridP = NewPos;
		}
	}

	for(uint32 SnakeIndex = 0; SnakeIndex < GameState->Level.SnakeCount; SnakeIndex++){
		ClearVisitedSnakes(GameState);
		uint32 SnakeID = SnakeIndex + SNAKE_ID_OFFSET; 

		vec2i CurrentHeadP = Level->Snakes[SnakeIndex].Parts[0].GridP;
		if(GameState->FruitRemaining == 0 && (GameState->PortalP.X == CurrentHeadP.X) && (GameState->PortalP.Y == CurrentHeadP.Y)){
			DeleteSnake(Level, SnakeID);
			if(GameState->Level.SnakeCount > 0 && SnakeID == GameState->Player->SnakeID){
				GameState->Player = &GameState->Level.Snakes[0];
			}else if(GameState->Level.SnakeCount == 0){
				GameState->LevelIndex = (GameState->LevelIndex+1)%LEVEL_COUNT;
				NewGame(GameState);
			}
			return;
		}
		while(IsRecursiveVisitPushable(GameState, SnakeID, SNAKE_MAX_COUNT+SNAKE_ID_OFFSET, vec2i{0, -1})){
			if(WillASnakeBePushedOnSpikes(GameState, vec2i{0, -1})){
				NewGame(GameState);
				return;
			}
			PushVisitedSnakes(GameState, &GameState->Level, vec2i{0, -1});
			ClearVisitedSnakes(GameState);
			
			if(GameState->FruitRemaining == 0 && (GameState->PortalP.X == CurrentHeadP.X) && (GameState->PortalP.Y == CurrentHeadP.Y)){
				DeleteSnake(Level, SnakeID);
				if(GameState->Level.SnakeCount > 0 && SnakeID == GameState->Player->SnakeID){
					GameState->Player = &GameState->Level.Snakes[0];
				}else if(GameState->Level.SnakeCount == 0){
					GameState->LevelIndex = (GameState->LevelIndex+1)%LEVEL_COUNT;
					NewGame(GameState);
				}
				return;
			}
		}
	}

	if(GameState->Level.Occupancy[GameState->PortalP.X][GameState->PortalP.Y] == Tile_Type_Empty){
		GameState->Level.Occupancy[GameState->PortalP.X][GameState->PortalP.Y] = Tile_Type_Goal;
	}
}

internal void
EditLevel(level *Level, SDL_Rect ScreenOutline, uint32 *ActiveBrush, game_input *Input){
	vec2i MouseGridP = GetGridP(Input->MouseX, Input->MouseY, ScreenOutline);
	if(IsInBounds(MouseGridP.X, MouseGridP.Y, Level->Width, Level->Height)){
		if(Input->MouseLeft.EndedDown){
			if(*ActiveBrush >= SNAKE_ID_OFFSET){
				if(Input->MouseLeft.Changed){
					if(Level->Occupancy[MouseGridP.X][MouseGridP.Y] >= SNAKE_ID_OFFSET){
						DeleteSnake(Level, Level->Occupancy[MouseGridP.X][MouseGridP.Y]);
					}
					snake NewSnake = snake{1, SNAKE_COLORS[*ActiveBrush-SNAKE_ID_OFFSET][0], SNAKE_COLORS[*ActiveBrush - SNAKE_ID_OFFSET][1]};
					NewSnake.Parts[0].GridP = vec2i{MouseGridP.X, MouseGridP.Y};
					AddSnakeToLevel(Level, NewSnake);
					Level->Occupancy[MouseGridP.X][MouseGridP.Y] = Level->Snakes[Level->SnakeCount-1].SnakeID;
				}else{
					snake *LastAddedSnake = &Level->Snakes[Level->SnakeCount - 1];
					bool32 SelfIntersecting  = false;
					for(uint32 p = 0; p < LastAddedSnake->Length; p++){
						vec2i GridP = LastAddedSnake->Parts[p].GridP;
						if(GridP.X == MouseGridP.X && GridP.Y == MouseGridP.Y){
							SelfIntersecting = true;
						}
					}
					if(!SelfIntersecting && LastAddedSnake->Length < SNAKE_MAX_LENGTH){
						if(Level->Occupancy[MouseGridP.X][MouseGridP.Y] >= SNAKE_ID_OFFSET){
							DeleteSnake(Level, Level->Occupancy[MouseGridP.X][MouseGridP.Y]);	
						}
						LastAddedSnake = &Level->Snakes[Level->SnakeCount - 1];
						LastAddedSnake->Parts[LastAddedSnake->Length++].GridP = vec2i{MouseGridP.X, MouseGridP.Y};
						Level->Occupancy[MouseGridP.X][MouseGridP.Y] = Level->Snakes[Level->SnakeCount-1].SnakeID;
					}
				}
			}
			
			if(*ActiveBrush <= 4 && Level->Occupancy[MouseGridP.X][MouseGridP.Y] < SNAKE_ID_OFFSET){
				Level->Occupancy[MouseGridP.X][MouseGridP.Y] = *ActiveBrush;
			}
		}else if(Input->MouseRight.EndedDown){
			if(Level->Occupancy[MouseGridP.X][MouseGridP.Y] >= SNAKE_ID_OFFSET){
				DeleteSnake(Level, Level->Occupancy[MouseGridP.X][MouseGridP.Y]);
			}else{
				Level->Occupancy[MouseGridP.X][MouseGridP.Y] = Tile_Type_Empty;
			}
		}
	}
	if(Input->Space.EndedDown && Input->Space.Changed){
		*ActiveBrush = ((*ActiveBrush)+1)%EDIT_BRUSH_COUNT ;
	}
}

internal void
TileLevel(game_state *GameState, SDL_Rect ScreenOutline, game_input *Input){
	vec2i MouseGridP = GetGridP(Input->MouseX, Input->MouseY, ScreenOutline);
	if(Input->MouseLeft.EndedDown){
		vec2i MouseDeltaP = {Input->MouseX-GameState->TileWindowRect.x, Input->MouseY-GameState->TileWindowRect.y};
		if(GameState->TileWindowActive && (MouseDeltaP.X >= 0 && MouseDeltaP.Y >= 0 && MouseDeltaP.X < GameState->TileWindowRect.w && MouseDeltaP.Y < GameState->TileWindowRect.h)){
			if(Input->MouseLeft.Changed){
				GameState->ActiveTileBrush = tile{GetTileGridRect(Input->MouseX, Input->MouseY, GameState->TileWindowRect)};
			}
		}else{
			GameState->Level.Tiles[GameState->ActiveLayerIndex][MouseGridP.X][MouseGridP.Y] = GameState->ActiveTileBrush;
		}
	}else if(Input->MouseRight.EndedDown){
		GameState->Level.Tiles[GameState->ActiveLayerIndex][MouseGridP.X][MouseGridP.Y] = {};
	}

	if(Input->Space.EndedDown && Input->Space.Changed){
		GameState->TileWindowActive = !GameState->TileWindowActive;
	}
	if(Input->ArrowLeft.EndedDown && Input->ArrowLeft.Changed){
		GameState->ActiveLayerIndex = CapInt32(0, GameState->ActiveLayerIndex - 1, LEVEL_MAX_LAYER_COUNT-1);
	}else if(Input->ArrowRight.EndedDown && Input->ArrowRight.Changed){
		GameState->ActiveLayerIndex = CapInt32(0, GameState->ActiveLayerIndex + 1, LEVEL_MAX_LAYER_COUNT-1);
	}
}

internal void
UpdateAndRender(game_memory *Memory, platform_state *Platform, game_input *Input)
{
	game_state *GameState = (game_state*)Memory->BaseAddress;
	if(GameState->MagicChecksum != 11789){
		GameState->MagicChecksum = 11789;
		InitGameState(GameState, (char*)Memory->BaseAddress + Memory->Size);
		SDL_SetRenderDrawBlendMode(Platform->Renderer, SDL_BLENDMODE_BLEND);
		LoadTileTextureAndAssignDimensions(GameState, Platform->Renderer, (char*)"tile_sheet.bmp");
		GameState->TileWindowRect = {0, SCREEN_HEIGHT - GameState->TextureHeight, GameState->TextureWidth, GameState->TextureHeight};
		NewGame(GameState);
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
			debug_read_file_result LevelHandle = DEBUGPlatformReadEntireFile((char*)TempName);
			if(LevelHandle.ContentsSize){
				memcpy(&GameState->Level, LevelHandle.Contents, LevelHandle.ContentsSize);
				DEBUGPlatformFreeFileMemory(LevelHandle.Contents);
			}
		}else{
			NewGame(GameState);
		}
	}
	if(Input->LeftCtrl.EndedDown && Input->s.EndedDown && Input->s.Changed){
		if(GameState->Mode == Game_Mode_Tile){
			GameState->Level.ForegroundLayerIndex = GameState->ActiveLayerIndex;
		}
		bool32 WriteSuccess = DEBUGPLatformWriteEntireFile((char*)TempName, sizeof(level), &GameState->Level);
		printf("Save Initiated, (1-success, 0-fail): %d  to %s\n", WriteSuccess, (char*)TempName);
	}
	
	//Simulation/Editing/Tiling
	if(GameState->Mode == Game_Mode_Play){
		UpdateLogic(GameState, Input);
	}else if(GameState->Mode == Game_Mode_Edit){
		EditLevel(&GameState->Level, Platform->ScreenOutline, &GameState->ActiveBrush, Input);
	}else{
		TileLevel(GameState, GameState->TileWindowRect, Input);
	}

	//Render
	SDL_SetRenderDrawColor(Platform->Renderer, 81, 180, 240, 255);
	SDL_RenderClear(Platform->Renderer);
	SDL_SetRenderDrawColor(Platform->Renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(Platform->Renderer, &Platform->ScreenOutline);

	if(GameState->Mode == Game_Mode_Play){
		DrawPlayModeElements(GameState, Platform->Renderer, Platform->ScreenOutline);
		DrawPlayModeLevel(GameState, Platform->Renderer, Platform->ScreenOutline, 0, GameState->Level.ForegroundLayerIndex);
		DrawSnakes(GameState, Platform->Renderer);
		DrawPlayModeLevel(GameState, Platform->Renderer, Platform->ScreenOutline, GameState->Level.ForegroundLayerIndex, LEVEL_MAX_LAYER_COUNT);
	} else if(GameState->Mode == Game_Mode_Edit){
		DrawEditModeLevel(GameState, Platform->Renderer, Platform->ScreenOutline);
		DrawSnakes(GameState, Platform->Renderer);
		DrawEditBrush(GameState, Platform->Renderer, Platform->ScreenOutline, Input->MouseX, Input->MouseY);
	}else if(GameState->Mode == Game_Mode_Tile){
		DrawTileModeLevelAndUI(GameState, Platform->Renderer, Platform->ScreenOutline);
		DrawTileBrush(GameState, Platform->Renderer, Platform->ScreenOutline, Input->MouseX, Input->MouseY);
	}
	SDL_RenderPresent(Platform->Renderer);
}

#endif //GAME_CPP
