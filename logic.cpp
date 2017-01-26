#if !defined(GAMEPLAY_LOGIC_CPP)
#define GAMEPLAY_LOGIC_CPP

internal void
PlayNextSnake(game_state *GameState){
	uint64 Index = GameState->Player - GameState->Level.Snakes;
	GameState->Player = &GameState->Level.Snakes[(Index+1) % GameState->Level.SnakeCount];
}

internal void
ClearMarkedSnakes(marked_snakes *MarkedSnakes){
	for(uint32 i = 0; i < SNAKE_MAX_COUNT; i++){
		MarkedSnakes->IsMarked[i] = false;
		MarkedSnakes->MarkedSnakes[i] = 0;
	}
	MarkedSnakes->Count = 0;
}

internal bool
IsRecursiveVisitPushable(level *Level, marked_snakes *MarkedSnakes, uint32 SnakeID, uint32 FirstSnakeID, vec2i Dir){
	uint32 SnakeIndex = SnakeID-SNAKE_ID_OFFSET;
	assert(SnakeIndex >= 0 && SnakeIndex < Level->SnakeCount);
	snake *Snake = Level->Snakes + SnakeIndex;

	MarkedSnakes->IsMarked[SnakeIndex] = true;
	MarkedSnakes->MarkedSnakes[MarkedSnakes->Count++] = Snake;

	for(int32 i = 0; i < Snake->Length; i++){
		vec2i DestP = Snake->Parts[i].GridP + Dir;
		if(!IsPointInBounds(DestP, Level->Width, Level->Height)){
			return false;
		}

		uint32 DestValue = Level->Occupancy[DestP.X][DestP.Y];
		if(	DestValue == FirstSnakeID || DestValue == Tile_Type_Solid ||
			DestValue == Tile_Type_Fruit || (DestValue == Tile_Type_Spikes && Dir.Y >= 0)){
			return false;
		}else if( 	DestValue >= SNAKE_ID_OFFSET && !MarkedSnakes->IsMarked[DestValue-SNAKE_ID_OFFSET] &&
					!IsRecursiveVisitPushable(Level, MarkedSnakes, DestValue, FirstSnakeID, Dir)){
			return false;
		}
	}
	return true;
}

internal bool
MarkIfSnakesCanMoove(level *Level, marked_snakes *MarkedSnakes, uint32 SnakeID, uint32 FirstSnakeID, vec2i Dir){
	ClearMarkedSnakes(MarkedSnakes);
	bool Result = IsRecursiveVisitPushable(Level, MarkedSnakes, SnakeID, FirstSnakeID, Dir);
	return Result;
}

internal void
PushMarkedSnakes(level *Level, marked_snakes *MarkedSnakes, vec2i Dir){
	for(int32 s = 0; s < MarkedSnakes->Count; s++){
		snake *Snake = MarkedSnakes->MarkedSnakes[s];
		for(int32 p = 0; p < Snake->Length; p++){
			vec2i PartPos = Snake->Parts[p].GridP;
			Level->Occupancy[PartPos.X][PartPos.Y] = Tile_Type_Empty;
		}
	}
	for(int32 s = 0; s < MarkedSnakes->Count; s++){
		snake *Snake = MarkedSnakes->MarkedSnakes[s];
		for(int32 p = 0; p < Snake->Length; p++){
			Snake->Parts[p].GridP += Dir;
			vec2i PartPos = Snake->Parts[p].GridP;
			Level->Occupancy[PartPos.X][PartPos.Y] = Snake->SnakeID;
		}
	}
}

internal bool
WillAMarkedSnakeBePushedOnSpikes(level *Level, marked_snakes *MarkedSnakes, vec2i Dir){
	for(int32 s = 0; s < MarkedSnakes->Count; s++){
		snake *Snake = MarkedSnakes->MarkedSnakes[s];
		for(int32 p = 0; p < Snake->Length; p++){
			vec2i DestDir = Snake->Parts[p].GridP + Dir;
			if(Level->Occupancy[DestDir.X][DestDir.Y] == Tile_Type_Spikes){
				return true;
			}
		}
	}
	return false;
}

internal void
GenerateNewStateAfterTransition(level *Level, marked_snakes *MarkedSnakes){
	bool PushOccured = false;
	for(uint32 SnakeIndex = 0; SnakeIndex < Level->SnakeCount; SnakeIndex++){
		snake *Snake = Level->Snakes + SnakeIndex;

		transition *Transition = &Snake->Transition;
		if(Transition->Type == Transition_Type_GotPushed && !PushOccured){
			PushMarkedSnakes(Level, MarkedSnakes, Transition->GotPushed.Direction);
			PushOccured = true;
		}
	}
	for(uint32 SnakeIndex = 0; SnakeIndex < Level->SnakeCount; SnakeIndex++){
		snake *Snake = Level->Snakes + SnakeIndex;
		snake_part *Head = Snake->Parts;
		snake_part *Tail = Snake->Parts + (Snake->Length-1);

		transition *Transition = &Snake->Transition;
		if(Transition->Type == Transition_Type_Slide){
			Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = Tile_Type_Empty;
			for(uint32 i = Snake->Length-1; i > 0; i--){
				Snake->Parts[i].GridP = Snake->Parts[i-1].GridP;
			}
			Head->GridP = Transition->Slide.NewHeadP;
			Level->Occupancy[Head->GridP.X][Head->GridP.Y] = Snake->SnakeID;
		}  
	}
}

internal void
UpdateLogic(game_state *GameState, game_input *Input, platform_service_v_table PlatformServices){
	if(!GameState->Player){
		return;
	}
	if(GameState->Transitioning){
		GameState->t += 0.15f;
		if(GameState->t <= 1.0f){
		}else{
			GameState->t = 0.0f;
			GameState->Transitioning = false;
			GenerateNewStateAfterTransition(&GameState->Level, &GameState->MarkedSnakes);
			for(uint32 i = 0; i < GameState->Level.SnakeCount; i++){
				GameState->Level.Snakes[i].Transition = {};
			}
		}
		return;
	}

	if(Input->Space.EndedDown && Input->Space.Changed){
		PlayNextSnake(GameState);
	}
	if(Input->r.EndedDown && Input->r.Changed){
		ReloadLevel(GameState, PlatformServices);
	}

	snake *Player = GameState->Player;
	level *Level = &GameState->Level;
	snake_part *Head = Player->Parts;

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
	
		if(Value == Tile_Type_Empty || Value == Tile_Type_Goal){
			Player->Transition.Type = Transition_Type_Slide;
			Player->Transition.Slide.NewHeadP = NewPos;
			GameState->Transitioning = true;
		}else if(Value >= SNAKE_ID_OFFSET && Value != Player->SnakeID && MarkIfSnakesCanMoove(Level, &GameState->MarkedSnakes, Value, Player->SnakeID, Direction)){
			for(int32 i = 0; i < GameState->MarkedSnakes.Count; i++){
				GameState->MarkedSnakes.MarkedSnakes[i]->Transition.Type = Transition_Type_GotPushed;
				GameState->MarkedSnakes.MarkedSnakes[i]->Transition.GotPushed.Direction = Direction;
			}
			Player->Transition.Type = Transition_Type_Slide;
			Player->Transition.Slide.NewHeadP = NewPos;
			GameState->Transitioning = true;
		}else if (Value == Tile_Type_Fruit){
			Player->Length++;
			assert(Player->Length <= SNAKE_MAX_LENGTH);
			GameState->FruitRemaining--;
			Player->Transition.Type = Transition_Type_Slide;
			Player->Transition.Slide.NewHeadP = NewPos;
			GameState->Transitioning = true;
		}
	}
	
#if 1
	if(!GameState->Transitioning){
		for(uint32 SnakeIndex = 0; SnakeIndex < Level->SnakeCount; SnakeIndex++){
			uint32 SnakeID = SnakeIndex + SNAKE_ID_OFFSET; 

			while(MarkIfSnakesCanMoove(Level, &GameState->MarkedSnakes, SnakeID, SNAKE_MAX_COUNT+SNAKE_ID_OFFSET, vec2i{0, -1})){
				if(WillAMarkedSnakeBePushedOnSpikes(Level, &GameState->MarkedSnakes, vec2i{0, -1})){
					ReloadLevel(GameState, PlatformServices);
					return;
				}
				PushMarkedSnakes(Level, &GameState->MarkedSnakes, vec2i{0, -1});
			}
		}
	}
#endif

#if 0
		vec2i CurrentHeadP = Level->Snakes[SnakeIndex].Parts[0].GridP;
		if(	GameState->FruitRemaining == 0 && (GameState->PortalP.X == CurrentHeadP.X) && (GameState->PortalP.Y == CurrentHeadP.Y)){
			DeleteSnakeReorderIDs(Level, SnakeID);
			if(Level->SnakeCount > 0 && SnakeID == GameState->Player->SnakeID){
				GameState->Player = &GameState->Level.Snakes[0];
			}else if(GameState->Level.SnakeCount == 0){
				GameState->LevelIndex = (GameState->LevelIndex+1)%GameState->LevelCount;
				ReloadLevel(GameState);
			}
			return;
		}
#endif
	if(Level->Occupancy[GameState->PortalP.X][GameState->PortalP.Y] == Tile_Type_Empty){
		Level->Occupancy[GameState->PortalP.X][GameState->PortalP.Y] = Tile_Type_Goal;
	}
}

#endif //GAMEPLAY_LOGIC_CPP
