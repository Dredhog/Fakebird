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

#if 0
internal void
internal inline void
DestroyAsteroid(game_state *GameState, uint32 Index) {
	assert(Index >= 0 && Index < GameState->AsteroidCount && GameState->AsteroidCount > 0);
	--GameState->AsteroidCount;
	GameState->SimAsteroids[Index] = GameState->SimAsteroids[GameState->AsteroidCount];
	GameState->ScreenAsteroids[Index] = GameState->ScreenAsteroids[GameState->AsteroidCount];
}
#endif

internal void
Drawboard(game_state *GameState, SDL_Renderer *Renderer){
	for(uint32 i = 0; i < GameState->Board.Width; i++){
		for(uint32 j = 0; j < GameState->Board.Height; j++){
			SDL_Rect CurrentRect = SDL_Rect{(int32)(i*BLOCK_WIDTH_IN_PIXELS),
											(int32)(SCREEN_HEIGHT - (j)*BLOCK_HEIGHT_IN_PIXELS),
											(int32)BLOCK_WIDTH_IN_PIXELS,
											(int32)BLOCK_HEIGHT_IN_PIXELS};

			if(GameState->Board.Occupancy[i][j] == 1){
				SDL_SetRenderDrawColor(Renderer, 154, 101, 55, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}else if(GameState->Board.Occupancy[i][j] == 2){
				SDL_SetRenderDrawColor(Renderer, 200, 200, 55, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
				SDL_RenderDrawRect(Renderer, &CurrentRect);
			}else if(GameState->Board.Occupancy[i][j] == 3){
				SDL_SetRenderDrawColor(Renderer, 154, 155, 155, 255);
				SDL_RenderFillRect(Renderer, &CurrentRect);
				SDL_SetRenderDrawColor(Renderer, 200, 150, 55, 255);
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
									(int32)(SCREEN_HEIGHT - (PartPos.Y)*BLOCK_HEIGHT_IN_PIXELS),
									(int32)BLOCK_WIDTH_IN_PIXELS,
									(int32)BLOCK_HEIGHT_IN_PIXELS};
			SDL_RenderFillRect(Renderer, &CurrentRect);
		}
		SDL_SetRenderDrawColor(Renderer, Snake->Color1.R, Snake->Color1.G, Snake->Color1.B, Snake->Color1.A);
		for(uint32 j = 1; j < Snake->Length; j+=2){
			vec2i PartPos =  Snake->Parts[j].GridP;
			SDL_Rect CurrentRect =	SDL_Rect{(int32)(PartPos.X*BLOCK_WIDTH_IN_PIXELS),
									(int32)(SCREEN_HEIGHT - (PartPos.Y)*BLOCK_HEIGHT_IN_PIXELS),
									(int32)BLOCK_WIDTH_IN_PIXELS,
									(int32)BLOCK_HEIGHT_IN_PIXELS};
			SDL_RenderFillRect(Renderer, &CurrentRect);
		}
	}
}

internal inline void
AddSnake(game_state *GameState, snake Snake) {
	assert(GameState->SnakeCount < GameState->SnakeCapacity);
	Snake.SnakeID = GameState->SnakeCount + SNAKE_ID_OFFSET;
	GameState->Snakes[GameState->SnakeCount++] = Snake;
}

internal inline void
PlayNextSnake(game_state *GameState){
	uint64 Index = GameState->Player - GameState->Snakes;
	GameState->Player = GameState->Snakes + ((Index+1) % GameState->SnakeCount);
}

internal void
InitGameState(game_state *GameState, void *MemoryEnd)
{
	assert(GameState);
	char *BaseAddress = ((char*)GameState) + sizeof(game_state);
	assert((BaseAddress + sizeof(game_state) + sizeof(snake)*SNAKE_MAX_COUNT) <= (char*)MemoryEnd);

	GameState->Snakes = (snake*)BaseAddress;

	GameState->SnakeCount 	= 0;
	GameState->VisitedCount = 0;
	GameState->SnakeCapacity = SNAKE_MAX_COUNT;
}

internal void
NewGame(game_state *GameState)
{
	GameState->Board.Width = EXAMPLE_LEVEL_WIDTH;
	GameState->Board.Height = EXAMPLE_LEVEL_HEIGHT;
	for(uint32 i = 0; i < GameState->Board.Width; i++){
		for(uint32 j = 0; j < GameState->Board.Height; j++){
			GameState->Board.Occupancy[i][GameState->Board.Height-j] = EXAMPLE_BOARD[j][i];
		}
	}

	GameState->SnakeCount = 0;
	GameState->VisitedCount   = 0;

	snake NewSnake = snake{4, color{255, 10, 10, 255}, color{255, 76, 48, 255}};
	NewSnake.Parts[0].GridP = vec2i{5, 3};
	NewSnake.Parts[1].GridP = vec2i{4, 3};
	NewSnake.Parts[2].GridP = vec2i{3, 3};
	NewSnake.Parts[3].GridP = vec2i{2, 3};
	AddSnake(GameState, NewSnake);

#if 1
	NewSnake = snake{4, color{0, 76, 255, 255}, color{0, 136, 255, 255}};
	NewSnake.Parts[0].GridP = vec2i{12, 2};
	NewSnake.Parts[1].GridP = vec2i{13, 2};
	NewSnake.Parts[2].GridP = vec2i{14, 2};
	NewSnake.Parts[3].GridP = vec2i{15, 2};
	AddSnake(GameState, NewSnake);
#endif

#if 1
	NewSnake = snake{2, color{24, 209, 31, 255}, color{71, 219, 72, 255}};
	NewSnake.Parts[0].GridP = vec2i{8, 8};
	NewSnake.Parts[1].GridP = vec2i{9, 8};
	AddSnake(GameState, NewSnake);
#endif

	GameState->Player = GameState->Snakes;
	for(uint32 i = 0; i < GameState->SnakeCount; i++){
		for(uint32 j = 0; j < GameState->Snakes[i].Length; j++){
			vec2i PartPos = GameState->Snakes[i].Parts[j].GridP;
			GameState->Board.Occupancy[PartPos.X][PartPos.Y] = GameState->Snakes[i].SnakeID;
		}
	}
}

internal inline uint32
ClampInt32(int32 a, int32 t, int32 b){
	if(t < a){
		return a;
	} else if (t > b){
		return b;
	}
	return t;
}

internal inline bool
IsInBounds(uint32 i, uint32 j, uint32 MaxI, uint32 MaxJ){
	if(i < 0 || j < 0 || i >= MaxI || j >= MaxI){
		return false;
	}	
	return true;
}

internal bool
IsRecursiveVisitPushable(game_state *GameState, uint32 SnakeID, uint32 FirstSnakeID, vec2i Dir){
	uint32 SnakeIndex = SnakeID-SNAKE_ID_OFFSET;
	assert(SnakeIndex >= 0 && SnakeIndex < GameState->SnakeCount);
	snake *Snake = GameState->Snakes + SnakeIndex;
	board *Board = &GameState->Board;

	GameState->Visited[SnakeIndex] = true;
	GameState->VisitedSnakes[GameState->VisitedCount++] = Snake;

	for(uint32 i = 0; i < Snake->Length; i++){
		vec2i DestPos = Snake->Parts[i].GridP + Dir;
		if(!IsInBounds(DestPos.X, DestPos.Y, Board->Width, Board->Height)){
			return false;
		}

		uint32 DestValue = Board->Occupancy[DestPos.X][DestPos.Y];
		if(	DestValue == FirstSnakeID || DestValue == 1 || DestValue == 2 || DestValue == 3){
			return false;
		} else if( 	DestValue >= SNAKE_ID_OFFSET && !GameState->Visited[DestValue-SNAKE_ID_OFFSET] &&
					!IsRecursiveVisitPushable(GameState, DestValue, FirstSnakeID, Dir)){
			return false;
		}
	}
	return true;
}

internal void
PushVisitedSnakes(game_state *GameState, board *Board, vec2i Dir){
	for(uint32 i = 0; i < GameState->VisitedCount; i++){
		snake *Snake = GameState->VisitedSnakes[i];
		for(uint32 i = 0; i < Snake->Length; i++){
			vec2i PartPos = Snake->Parts[i].GridP;
			Board->Occupancy[PartPos.X][PartPos.Y] = 0;
			Snake->Parts[i].GridP += Dir;
		}
	}
	for(uint32 i = 0; i < GameState->VisitedCount; i++){
		snake *Snake = GameState->VisitedSnakes[i];
		for(uint32 i = 0; i < Snake->Length; i++){
			vec2i PartPos = Snake->Parts[i].GridP;
			Board->Occupancy[PartPos.X][PartPos.Y] = Snake->SnakeID;
		}
	}
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
	assert(GameState->Player);

	if(Input->Space.EndedDown && Input->Space.Changed){
		PlayNextSnake(GameState);
	}
	if(Input->MouseLeft.EndedDown && Input->MouseLeft.Changed){
		NewGame(GameState);
	}

	snake *Player = GameState->Player;
	board *Board = &GameState->Board;
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
	if(IsInBounds(NewPos.X, NewPos.Y, Board->Width, Board->Height)){
		uint32 Value = Board->Occupancy[NewPos.X][NewPos.Y];

		if(Value == 0){
			Board->Occupancy[Tail->GridP.X][Tail->GridP.Y] = 0;
			for(uint32 i = Player->Length-1; i > 0; i--){
				Player->Parts[i].GridP = Player->Parts[i-1].GridP;
			}
			Board->Occupancy[NewPos.X][NewPos.Y] = Player->SnakeID;
			Head->GridP = NewPos;
		}else if(Value >= SNAKE_ID_OFFSET && Value != Player->SnakeID && IsRecursiveVisitPushable(GameState, Value, Player->SnakeID, Direction)){
			PushVisitedSnakes(GameState, &GameState->Board, Direction);
			Board->Occupancy[Tail->GridP.X][Tail->GridP.Y] = 0;
			for(uint32 i = Player->Length-1; i > 0; i--){
				Player->Parts[i].GridP = Player->Parts[i-1].GridP;
			}
			Board->Occupancy[NewPos.X][NewPos.Y] = Player->SnakeID;
			Head->GridP = NewPos;
		}else if(Value == 2){
			Player->Length++;
			assert(Player->Length <= SNAKE_MAX_LENGTH);
			for(uint32 i = Player->Length-1; i > 0; i--){
				Player->Parts[i].GridP = Player->Parts[i-1].GridP;
			}
			Board->Occupancy[NewPos.X][NewPos.Y] = Player->SnakeID;
			Head->GridP = NewPos;
		}
	}
	for(uint32 SnakeIndex = 0; SnakeIndex < GameState->SnakeCount; SnakeIndex++){
		ClearVisitedSnakes(GameState);
		while(IsRecursiveVisitPushable(GameState, SnakeIndex + SNAKE_ID_OFFSET, 8, vec2i{0, -1})){
			PushVisitedSnakes(GameState, &GameState->Board, vec2i{0, -1});
			ClearVisitedSnakes(GameState);
		}
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

	Drawboard(GameState, Platform->Renderer);
	DrawSnakes(GameState, Platform->Renderer);

	SDL_RenderPresent(Platform->Renderer);
}

#endif //GAME_CPP
