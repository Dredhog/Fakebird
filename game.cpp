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
DrawLevel(game_state *GameState, SDL_Renderer *Renderer){
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
}

internal void
DrawSnakes(game_state *GameState, SDL_Renderer *Renderer){
	for(uint32 SnakeIndex = 0; SnakeIndex  < GameState->SnakeCount; SnakeIndex++){
		snake *Snake = GameState->Snakes + SnakeIndex;

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
}

internal void
PlayNextSnake(game_state *GameState){
	uint64 Index = GameState->Player - GameState->Snakes;
	GameState->Player = GameState->Snakes + ((Index+1) % GameState->SnakeCount);
}

internal void
DeleteSnake(game_state *GameState, uint32 SnakeID){
	uint32 RemoveIndex = SnakeID - SNAKE_ID_OFFSET;
	assert(RemoveIndex >= 0 && RemoveIndex < GameState->SnakeCount);
	for(uint32 p = 0; p < GameState->Snakes[RemoveIndex].Length; p++){
		vec2i PartP = GameState->Snakes[RemoveIndex].Parts[p].GridP;
		GameState->Level.Occupancy[PartP.X][PartP.Y] = 0;
	}
	for(uint32 i = RemoveIndex; i < GameState->SnakeCount-1; i++){
		GameState->Snakes[i] = GameState->Snakes[i+1];
	}
	GameState->SnakeCount--;

	for(uint32 i = 0; i < GameState->SnakeCount; i++){
		GameState->Snakes[i].SnakeID = i + SNAKE_ID_OFFSET;
		for(uint32 p = 0; p < GameState->Snakes[i].Length; p++){
			vec2i PartP = GameState->Snakes[i].Parts[p].GridP;
			GameState->Level.Occupancy[PartP.X][PartP.Y] = GameState->Snakes[i].SnakeID;
		}
	}
}

internal void
InitGameState(game_state *GameState, void *MemoryEnd)
{
	assert(GameState);
	char *BaseAddress = ((char*)GameState) + sizeof(game_state);
	assert((BaseAddress + sizeof(game_state) + sizeof(snake)*SNAKE_MAX_COUNT) <= (char*)MemoryEnd);

	GameState->Snakes = (snake*)BaseAddress;

	GameState->LevelIndex = 2;
	GameState->SnakeCapacity = SNAKE_MAX_COUNT;

	snake NewSnake;
	NewSnake = snake{8, color{255, 10, 10, 255}, color{255, 76, 48, 255}};
	NewSnake.Parts[0].GridP = vec2i{9, 6};
	NewSnake.Parts[1].GridP = vec2i{8, 6};
	NewSnake.Parts[2].GridP = vec2i{8, 5};
	NewSnake.Parts[3].GridP = vec2i{8, 4};
	NewSnake.Parts[4].GridP = vec2i{9, 4};
	NewSnake.Parts[5].GridP = vec2i{10, 4};
	NewSnake.Parts[6].GridP = vec2i{10, 3};
	NewSnake.Parts[7].GridP = vec2i{10, 2};
	AddSnakeToLevel(&GameState->Levels[0], NewSnake);

	NewSnake = snake{3, color{0, 76, 255, 255}, color{0, 136, 255, 255}};
	NewSnake.Parts[0].GridP = vec2i{10, 4};
	NewSnake.Parts[1].GridP = vec2i{9, 4};
	NewSnake.Parts[2].GridP = vec2i{9, 3};
	AddSnakeToLevel(&GameState->Levels[1], NewSnake);

	NewSnake = snake{3, color{24, 209, 31, 255}, color{71, 219, 72, 255}};
	NewSnake.Parts[0].GridP = vec2i{12, 4};
	NewSnake.Parts[1].GridP = vec2i{13, 4};
	NewSnake.Parts[2].GridP = vec2i{14, 4};
	AddSnakeToLevel(&GameState->Levels[2], NewSnake);

	NewSnake = snake{4, color{0, 76, 255, 255}, color{0, 136, 255, 255}};
	NewSnake.Parts[0].GridP = vec2i{13, 5};
	NewSnake.Parts[1].GridP = vec2i{14, 5};
	NewSnake.Parts[2].GridP = vec2i{15, 5};
	NewSnake.Parts[3].GridP = vec2i{15, 4};
	AddSnakeToLevel(&GameState->Levels[2], NewSnake);
}

internal void
NewGame(game_state *GameState)
{
	GameState->VisitedCount   = 0;
	GameState->Level = GameState->Levels[GameState->LevelIndex];
	GameState->Level.Width = EXAMPLE_LEVEL_WIDTH;
	GameState->Level.Height = EXAMPLE_LEVEL_HEIGHT;

	GameState->FruitRemaining = 0;
	for(uint32 i = 0; i < GameState->Level.Width; i++){
		for(uint32 j = 0; j < GameState->Level.Height; j++){
			uint32 Value = LEVELS[GameState->LevelIndex][(GameState->Level.Height-1)-j][i];
			GameState->Level.Occupancy[i][j] = Value;
			if(Value == 2){
				GameState->FruitRemaining++;
			}else if(Value == 4){
				GameState->PortalP = vec2i{(int32)i, (int32)j};
			}
		}
	}


	GameState->SnakeCount = GameState->Level.SnakeCount;
	for(uint32 i = 0; i < GameState->Level.SnakeCount; i++){
		GameState->Snakes[i] = GameState->Level.Snakes[i];
	}
	GameState->Player = GameState->Snakes;

	for(uint32 i = 0; i < GameState->SnakeCount; i++){
		for(uint32 j = 0; j < GameState->Snakes[i].Length; j++){
			vec2i PartPos = GameState->Snakes[i].Parts[j].GridP;
			GameState->Level.Occupancy[PartPos.X][PartPos.Y] = GameState->Snakes[i].SnakeID;
		}
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
	assert(SnakeIndex >= 0 && SnakeIndex < GameState->SnakeCount);
	snake *Snake = GameState->Snakes + SnakeIndex;
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
	for(uint64 SnakeIndex = 0; SnakeIndex < GameState->SnakeCount; SnakeIndex++){
		snake *Snake = GameState->Snakes + SnakeIndex;
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
ClearVisitedSnakes(game_state *GameState){
	GameState->VisitedCount = 0;
	for(uint32 i = 0; i < GameState->SnakeCount; i++){
		GameState->Visited[i] = false;
	}
}

internal void
UpdateLogic(game_state *GameState, game_input *Input){
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

	vec2i Direction;
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
				DeleteSnake(GameState, Player->SnakeID);
				if(GameState->SnakeCount > 0){
					GameState->Player = GameState->Snakes;
				}else if(GameState->SnakeCount == 0){
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
	for(uint32 SnakeIndex = 0; SnakeIndex < GameState->SnakeCount; SnakeIndex++){
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
	UpdateLogic(GameState, Input);

	//Render
	SDL_SetRenderDrawColor(Platform->Renderer, 81, 180, 240, 255);
	SDL_RenderClear(Platform->Renderer);

	SDL_SetRenderDrawColor(Platform->Renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(Platform->Renderer, &Platform->screen_outline);

	DrawLevel(GameState, Platform->Renderer);
	DrawSnakes(GameState, Platform->Renderer);

	SDL_RenderPresent(Platform->Renderer);
}

#endif //GAME_CPP
