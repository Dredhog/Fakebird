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

internal void
DrawLevel(game_state *GameState, platform_state *Platform, int32 MouseX, int32 MouseY){
	SDL_Renderer *Renderer = Platform->Renderer;

	SDL_SetRenderDrawColor(Renderer, 81, 180, 240, 255);
	SDL_RenderClear(Renderer);

	SDL_SetRenderDrawColor(Platform->Renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(Renderer, &Platform->screen_outline);

	for(uint32 i = 0; i < GameState->Level.Width; i++){
		for(uint32 j = 0; j < GameState->Level.Height; j++){
			SDL_Rect CurrentRect = SDL_Rect{(int32)(i*BLOCK_WIDTH_IN_PIXELS),
											(int32)(SCREEN_HEIGHT - (j+1)*BLOCK_HEIGHT_IN_PIXELS),
											(int32)BLOCK_WIDTH_IN_PIXELS,
											(int32)BLOCK_HEIGHT_IN_PIXELS};

			if(GameState->Level.Occupancy[i][j] == 1){
				SDL_SetRenderDrawColor(Renderer, 154, 101, 55, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}else if(GameState->Level.Occupancy[i][j] == 2){
				SDL_SetRenderDrawColor(Renderer, 200, 200, 55, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}else if(GameState->Level.Occupancy[i][j] == 3){
				SDL_SetRenderDrawColor(Renderer, 154, 155, 155, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}else if(GameState->Level.Occupancy[i][j] == 4 && GameState->FruitRemaining != 0){
				SDL_SetRenderDrawColor(Renderer, 70, 50, 70, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}else if(GameState->Level.Occupancy[i][j] == 4 && GameState->FruitRemaining == 0){
				SDL_SetRenderDrawColor(Renderer, 255, 50, 200, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}else if(GameState->Level.Occupancy[i][j] == 0){

			}else{
				SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}
		}
	}
	if(GameState->EditModeActive){
		int32 MouseGridX = (int32)((real32)MouseX/(real32)BLOCK_HEIGHT_IN_PIXELS);
	   	int32 MouseGridY = (int32)((real32)(SCREEN_HEIGHT-MouseY)/(real32)BLOCK_WIDTH_IN_PIXELS);

		SDL_Rect CurrentRect = SDL_Rect{(int32)(MouseGridX*BLOCK_WIDTH_IN_PIXELS),
										(int32)(SCREEN_HEIGHT - (MouseGridY+1)*BLOCK_HEIGHT_IN_PIXELS),
										(int32)BLOCK_WIDTH_IN_PIXELS,
										(int32)BLOCK_HEIGHT_IN_PIXELS};
		switch(GameState->ActiveBrush){
			case 0:
				break;
			case 1:
				SDL_SetRenderDrawColor(Renderer, 154, 101, 55, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
				break;
			case 2:
				SDL_SetRenderDrawColor(Renderer, 200, 200, 55, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
				break;
			case 3:
				SDL_SetRenderDrawColor(Renderer, 154, 155, 155, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
				break;
			case 4:
				SDL_SetRenderDrawColor(Renderer, 255, 50, 200, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
				break;
			default:
			if(GameState->ActiveBrush >= SNAKE_ID_OFFSET && GameState->ActiveBrush < SNAKE_ID_OFFSET + 3){
				color SnakeColor = SNAKE_COLORS[GameState->ActiveBrush-SNAKE_ID_OFFSET][0];
				SDL_SetRenderDrawColor(Renderer, SnakeColor.R, SnakeColor.G, SnakeColor.B, SnakeColor.A);
				SDL_RenderFillRect(Renderer, &CurrentRect);
			}
		}
	}
}

internal void
DrawSnakes(game_state *GameState, SDL_Renderer *Renderer){
	for(uint32 SnakeIndex = 0; SnakeIndex  < GameState->Level.SnakeCount; SnakeIndex++){
		snake *Snake = &GameState->Level.Snakes[SnakeIndex];

		SDL_SetRenderDrawColor(Renderer, Snake->Color0.R, Snake->Color0.G, Snake->Color0.B, Snake->Color0.A);
		for(uint32 j = 0; j < Snake->Length; j+=2){
			vec2i PartPos =  Snake->Parts[j].GridP;
			SDL_Rect CurrentRect =  SDL_Rect{(int32)(PartPos.X*BLOCK_WIDTH_IN_PIXELS),
									(int32)(SCREEN_HEIGHT - (PartPos.Y+1)*BLOCK_HEIGHT_IN_PIXELS),
									(int32)BLOCK_WIDTH_IN_PIXELS,
									(int32)BLOCK_HEIGHT_IN_PIXELS};
			SDL_RenderFillRect(Renderer, &CurrentRect);
		}
		SDL_SetRenderDrawColor(Renderer, Snake->Color1.R, Snake->Color1.G, Snake->Color1.B, Snake->Color1.A);
		for(uint32 j = 1; j < Snake->Length; j+=2){
			vec2i PartPos =  Snake->Parts[j].GridP;
			SDL_Rect CurrentRect =	SDL_Rect{(int32)(PartPos.X*BLOCK_WIDTH_IN_PIXELS),
									(int32)(SCREEN_HEIGHT - (PartPos.Y+1)*BLOCK_HEIGHT_IN_PIXELS),
									(int32)BLOCK_WIDTH_IN_PIXELS,
									(int32)BLOCK_HEIGHT_IN_PIXELS};
			SDL_RenderFillRect(Renderer, &CurrentRect);
		}
	}
}


internal void
AddSnakeToLevel(level *Level, snake Snake){
	assert(Level->SnakeCount >= 0 && Level->SnakeCount < SNAKE_MAX_COUNT);
	Snake.SnakeID = Level->SnakeCount + SNAKE_ID_OFFSET;
	Level->Snakes[Level->SnakeCount++] = Snake;
	for(uint32 i = 0; i < Snake.Length; i++){
		vec2i PartP = Snake.Parts[i].GridP;
		Level->Occupancy[PartP.X][PartP.Y] = Snake.SnakeID;
	}
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
		Level->Occupancy[PartP.X][PartP.Y] = 0;
	}
	for(uint32 i = RemoveIndex; i < Level->SnakeCount-1; i++){
		Level->Snakes[i] = Level->Snakes[i+1];
	}
	Level->SnakeCount--;

	for(uint32 i = 0; i < Level->SnakeCount; i++){
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
	GameState->EditModeActive = true;
	GameState->ActiveBrush = 1;
}

internal void
ClearVisitedSnakes(game_state *GameState){
	GameState->VisitedCount = 0;
	for(uint32 i = 0; i < GameState->Level.SnakeCount; i++){
		GameState->Visited[i] = false;
	}
}


internal void
NewGame(game_state *GameState)
{
	debug_read_file_result LevelHandle = DEBUGPlatformReadEntireFile((char*)TempName);
	if(LevelHandle.ContentsSize){
		memcpy(&GameState->Level, LevelHandle.Contents, LevelHandle.ContentsSize);
		DEBUGPlatformFreeFileMemory(LevelHandle.Contents);
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
		ClearVisitedSnakes(GameState);
	}else{
		GameState->Level = {}; 
		GameState->Player = {};
		GameState->Level.Width = LEVEL_MAX_WIDTH;
		GameState->Level.Height = LEVEL_MAX_HEIGHT;
	}

}

internal inline bool
IsInBounds(int32 i, int32 j, int32 MaxI, int32 MaxJ){
	if(i < 0 || j < 0 || i >= MaxI || j >= MaxJ){
		return false;
	}	
	return true;
}

internal bool
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
	for(uint32 i = 0; i < GameState->VisitedCount; i++){
		snake *Snake = GameState->VisitedSnakes[i];
		for(uint32 i = 0; i < Snake->Length; i++){
			vec2i PartPos = Snake->Parts[i].GridP;
			Level->Occupancy[PartPos.X][PartPos.Y] = 0;
			Snake->Parts[i].GridP += Dir;
		}
	}
	for(uint32 i = 0; i < GameState->VisitedCount; i++){
		snake *Snake = GameState->VisitedSnakes[i];
		for(uint32 i = 0; i < Snake->Length; i++){
			vec2i PartPos = Snake->Parts[i].GridP;
			Level->Occupancy[PartPos.X][PartPos.Y] = Snake->SnakeID;
		}
	}
}

internal bool
WillSnakesBeOnSpikes(game_state *GameState, vec2i Dir){
	for(uint64 SnakeIndex = 0; SnakeIndex < GameState->Level.SnakeCount; SnakeIndex++){
		snake *Snake = GameState->Level.Snakes + SnakeIndex;
		for(uint32 i = 0; i < Snake->Length; i++){
			vec2i P = Snake->Parts[i].GridP + Dir;
			if(GameState->Level.Occupancy[P.X][P.Y] == 3){
				return true;
			}
		}
	}
	return false;
}

internal void
EditLevel(game_state *GameState, game_input *Input){
	level *Level = &GameState->Level;

	int32 MouseGridX = (int32)((real32)Input->MouseX/(real32)BLOCK_HEIGHT_IN_PIXELS);
	int32 MouseGridY = (int32)((real32)(SCREEN_HEIGHT-Input->MouseY)/(real32)BLOCK_WIDTH_IN_PIXELS);
	if(IsInBounds(MouseGridX, MouseGridY, Level->Width, Level->Height)){
		if(Input->MouseLeft.EndedDown){
			if(GameState->ActiveBrush >= SNAKE_ID_OFFSET){
				if(Input->MouseLeft.Changed){
					if(Level->Occupancy[MouseGridX][MouseGridY] >= SNAKE_ID_OFFSET){
						DeleteSnake(Level, Level->Occupancy[MouseGridX][MouseGridY]);
					}
					snake NewSnake = snake{1, SNAKE_COLORS[GameState->ActiveBrush-SNAKE_ID_OFFSET][0], SNAKE_COLORS[GameState->ActiveBrush - SNAKE_ID_OFFSET][1]};
					NewSnake.Parts[0].GridP = vec2i{MouseGridX, MouseGridY};
					AddSnakeToLevel(Level, NewSnake);
					Level->Occupancy[MouseGridX][MouseGridY] = Level->Snakes[Level->SnakeCount-1].SnakeID;
				}else{
					snake *LastAddedSnake = &Level->Snakes[Level->SnakeCount - 1];
					bool32 SelfIntersecting  = false;
					for(uint32 p = 0; p < LastAddedSnake->Length; p++){
						vec2i GridP = LastAddedSnake->Parts[p].GridP;
						if(GridP.X == MouseGridX && GridP.Y == MouseGridY){
							SelfIntersecting = true;
						}
					}
					if(!SelfIntersecting && LastAddedSnake->Length < SNAKE_MAX_LENGTH){
						if(Level->Occupancy[MouseGridX][MouseGridY] >= SNAKE_ID_OFFSET){
							DeleteSnake(Level, Level->Occupancy[MouseGridX][MouseGridY]);	
						}
						LastAddedSnake = &Level->Snakes[Level->SnakeCount - 1];
						LastAddedSnake->Parts[LastAddedSnake->Length++].GridP = vec2i{MouseGridX, MouseGridY};
						Level->Occupancy[MouseGridX][MouseGridY] = Level->Snakes[Level->SnakeCount-1].SnakeID;
					}
				}
			}
			
			if(GameState->ActiveBrush <= 4 && Level->Occupancy[MouseGridX][MouseGridY] < SNAKE_ID_OFFSET){
				Level->Occupancy[MouseGridX][MouseGridY] = GameState->ActiveBrush;
			}
		}else if(Input->MouseRight.EndedDown){
			if(Level->Occupancy[MouseGridX][MouseGridY] >= SNAKE_ID_OFFSET){
				DeleteSnake(Level, Level->Occupancy[MouseGridX][MouseGridY]);
			}else{
				Level->Occupancy[MouseGridX][MouseGridY] = 0;
			}
		}
	}
	if(Input->n.EndedDown && Input->n.Changed){
		GameState->ActiveBrush = (GameState->ActiveBrush+1)%8;
	}
	if(Input->LeftCtrl.EndedDown && Input->s.EndedDown && Input->s.Changed){
		bool32 WriteSuccess = DEBUGPLatformWriteEntireFile((char*)TempName, sizeof(level), Level);
		printf("Save Initiated, (1-success, 0-fail): %d  to %s\n", WriteSuccess, (char*)TempName);
	}
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
	
	ClearVisitedSnakes(GameState);
	
	vec2i NewPos = Head->GridP + Direction;
	if(IsInBounds(NewPos.X, NewPos.Y, Level->Width, Level->Height)){
		uint32 Value = Level->Occupancy[NewPos.X][NewPos.Y];

		bool move = false;
		if(Value == 0){
			Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = 0;
			move = true;
		}else if(Value >= SNAKE_ID_OFFSET && Value != Player->SnakeID && IsRecursiveVisitPushable(GameState, Value, Player->SnakeID, Direction)){
			PushVisitedSnakes(GameState, &GameState->Level, Direction);
			Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = 0;
			move = true;
		}else if(Value == 2){
			Player->Length++;
			assert(Player->Length <= SNAKE_MAX_LENGTH);
			GameState->FruitRemaining--;
			move = true;
		}else if(Value == 4){
			if(GameState->FruitRemaining == 0){
				DeleteSnake(Level, Player->SnakeID);
				if(GameState->Level.SnakeCount > 0){
					GameState->Player = GameState->Level.Snakes;
				}else if(GameState->Level.SnakeCount == 0){
					GameState->LevelIndex = (GameState->LevelIndex+1)%LEVEL_COUNT;
					NewGame(GameState);
				}
				return;
			}
			Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = 0;
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
		while(IsRecursiveVisitPushable(GameState, SnakeIndex + SNAKE_ID_OFFSET, SNAKE_MAX_COUNT+SNAKE_ID_OFFSET, vec2i{0, -1})){
			if(WillSnakesBeOnSpikes(GameState, vec2i{0, -1})){
				NewGame(GameState);
				return;
			}
			PushVisitedSnakes(GameState, &GameState->Level, vec2i{0, -1});
			ClearVisitedSnakes(GameState);
		}
	}

	if(GameState->Level.Occupancy[GameState->PortalP.X][GameState->PortalP.Y] == 0){
		GameState->Level.Occupancy[GameState->PortalP.X][GameState->PortalP.Y] = 4;
	}

}

void
UpdateAndRender(game_memory *Memory, platform_state *Platform, game_input *Input)
{
	game_state *GameState = (game_state*)Memory->BaseAddress;
	if(GameState->MagicChecksum != 11789){
		GameState->MagicChecksum = 11789;
		InitGameState(GameState, (char*)Memory->BaseAddress + Memory->Size);
		NewGame(GameState);
	}

	//Logic
	if(Input->e.EndedDown && Input->e.Changed){
		GameState->EditModeActive = !GameState->EditModeActive;
		if(!GameState->EditModeActive){
			NewGame(GameState);
		}else{
			debug_read_file_result LevelHandle = DEBUGPlatformReadEntireFile((char*)TempName);
			if(LevelHandle.ContentsSize){
				memcpy(&GameState->Level, LevelHandle.Contents, LevelHandle.ContentsSize);
				DEBUGPlatformFreeFileMemory(LevelHandle.Contents);
			}
		}
	}
	if(GameState->EditModeActive){
		EditLevel(GameState, Input);
	}else{
		UpdateLogic(GameState, Input);
	}

	//Render
	DrawLevel(GameState, Platform, Input->MouseX, Input->MouseY);
	DrawSnakes(GameState, Platform->Renderer);

	SDL_RenderPresent(Platform->Renderer);
}

#endif //GAME_CPP
