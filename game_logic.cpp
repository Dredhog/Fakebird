#if !defined(GAMEPLAY_LOGIC_CPP)
#define GAMEPLAY_LOGIC_CPP

internal void
PlayNextSnake(game_state *GameState){
	uint64 Index = GameState->Player - GameState->Level.Snakes;
	GameState->Player = &GameState->Level.Snakes[(Index+1) % GameState->Level.SnakeCount];
}

internal void
ClearVisitedSnakes(game_state *GameState){
	GameState->VisitedCount = 0;
	for(uint32 i = 0; i < GameState->Level.SnakeCount; i++){
		GameState->Visited[i] = false;
		GameState->VisitedSnakes[i] = 0;
	}
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
		if(!IsPointInBounds(DestP, Level->Width, Level->Height)){
			return false;
		}

		uint32 DestValue = Level->Occupancy[DestP.X][DestP.Y];
		if(	DestValue == FirstSnakeID || DestValue == Tile_Type_Solid ||
			DestValue == Tile_Type_Fruit || (DestValue == Tile_Type_Spikes && Dir.Y >= 0)){
			return false;
		}else if( 	DestValue >= SNAKE_ID_OFFSET && !GameState->Visited[DestValue-SNAKE_ID_OFFSET] &&
					!IsRecursiveVisitPushable(GameState, DestValue, FirstSnakeID, Dir)){
			return false;
		}
	}
	return true;
}

internal bool
MarkIfSnakesCanMoove(game_state *GameState, uint32 SnakeID, uint32 FirstSnakeID, vec2i Dir){
	ClearVisitedSnakes(GameState);
	bool Result = IsRecursiveVisitPushable(GameState, SnakeID, FirstSnakeID, Dir);
	return Result;
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
		ReloadLevel(GameState);
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
	
		bool32 move = false;
		if(Value == Tile_Type_Empty || Value == Tile_Type_Goal){
			Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = Tile_Type_Empty;
			move = true;
		}else if(Value >= SNAKE_ID_OFFSET && Value != Player->SnakeID && MarkIfSnakesCanMoove(GameState, Value, Player->SnakeID, Direction)){
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
		uint32 SnakeID = SnakeIndex + SNAKE_ID_OFFSET; 

		vec2i CurrentHeadP = Level->Snakes[SnakeIndex].Parts[0].GridP;
		if(GameState->FruitRemaining == 0 && (GameState->PortalP.X == CurrentHeadP.X) && (GameState->PortalP.Y == CurrentHeadP.Y)){
			DeleteSnakeReorderIDs(Level, SnakeID);
			if(GameState->Level.SnakeCount > 0 && SnakeID == GameState->Player->SnakeID){
				GameState->Player = &GameState->Level.Snakes[0];
			}else if(GameState->Level.SnakeCount == 0){
				GameState->LevelIndex = (GameState->LevelIndex+1)%LEVEL_COUNT;
				ReloadLevel(GameState);
			}
			return;
		}
		while(MarkIfSnakesCanMoove(GameState, SnakeID, SNAKE_MAX_COUNT+SNAKE_ID_OFFSET, vec2i{0, -1})){
			if(WillASnakeBePushedOnSpikes(GameState, vec2i{0, -1})){
				ReloadLevel(GameState);
				return;
			}
			PushVisitedSnakes(GameState, &GameState->Level, vec2i{0, -1});
		}
	}
				

	if(Level->Occupancy[GameState->PortalP.X][GameState->PortalP.Y] == Tile_Type_Empty){
		Level->Occupancy[GameState->PortalP.X][GameState->PortalP.Y] = Tile_Type_Goal;
	}
}

#endif //GAMEPLAY_LOGIC_CPP
