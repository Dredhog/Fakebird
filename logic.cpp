#if !defined(GAMEPLAY_LOGIC_CPP)
#define GAMEPLAY_LOGIC_CPP

internal void
UpdateOverworld(game_state *GameState, rectangle ScreenOutline, rectangle GameBoardRect, platform_services PlatformServices, game_input *Input){
	int32 MouseRectIndex = GetOverworldRectIndex(Input->MouseX, Input->MouseY, GameBoardRect);
	if(Input->MouseLeft.Changed && Input->MouseLeft.EndedDown){
		if(MouseRectIndex >= 0){
			GameState->LevelIndex = MouseRectIndex;
			ReloadLevel(GameState, PlatformServices);
			GameState->Mode = Game_Mode_Play;
		}
	}
	if(Input->MouseRight.Changed && Input->MouseRight.EndedDown){
		if(MouseRectIndex >= 0){
			GameState->Overworld.LevelInfos[MouseRectIndex].Exists = false;
		}
	}
}

internal void
PlayNextSnake(game_state *GameState){
	uint64 Index = GameState->Player - GameState->Level.Snakes;
	GameState->Player = &GameState->Level.Snakes[(Index+1) % GameState->Level.SnakeCount];
}

internal void
ClearMarkedSnakes(marked_snakes *MarkedSnakes){
	for(uint32 i = 0; i < ArrayCount(MarkedSnakes->IsMarked); i++){
		MarkedSnakes->IsMarked[i] = false;
		MarkedSnakes->Snakes[i] = 0;
	}
	MarkedSnakes->Count = 0;
}

internal bool
IsRecursiveVisitPushable(level *Level, marked_snakes *MarkedSnakes, uint32 SnakeID, uint32 FirstSnakeID, vec2i Dir){
	uint32 SnakeIndex = SnakeID-SNAKE_ID_OFFSET;
	assert(SnakeIndex >= 0 && SnakeIndex < Level->SnakeCount);
	snake *Snake = Level->Snakes + SnakeIndex;

	MarkedSnakes->IsMarked[SnakeIndex] = true;
	MarkedSnakes->Snakes[MarkedSnakes->Count++] = Snake;

	for(int32 i = 0; i < Snake->Length; i++){
		vec2i DestP = Snake->Parts[i].GridP + Dir;
		if(!IsPInBounds(DestP, Level->Width, Level->Height)){
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
		snake *Snake = MarkedSnakes->Snakes[s];
		for(int32 p = 0; p < Snake->Length; p++){
			vec2i PartPos = Snake->Parts[p].GridP;
			Level->Occupancy[PartPos.X][PartPos.Y] = Tile_Type_Empty;
		}
	}
	for(int32 s = 0; s < MarkedSnakes->Count; s++){
		snake *Snake = MarkedSnakes->Snakes[s];
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
		snake *Snake = MarkedSnakes->Snakes[s];
		for(int32 p = 0; p < Snake->Length; p++){
			vec2i DestDir = Snake->Parts[p].GridP + Dir;
			if(Level->Occupancy[DestDir.X][DestDir.Y] == Tile_Type_Spikes){
				return true;
			}
		}
	}
	return false;
}

internal uint32
IfOnPortalGetPortalID(level *Level, uint32 SnakeID){
	uint32 SnakeIndex = SnakeID - SNAKE_ID_OFFSET;
	assert(SnakeIndex >= 0 && SnakeIndex < Level->SnakeCount); 
	snake *Snake = Level->Snakes + SnakeIndex;
	for(int32 p = 0; p < Snake->Length; p++){
		vec2i PartP = Snake->Parts[p].GridP;
		for(int32 PortalIndex = 0; PortalIndex < 2; PortalIndex++){
			if(PartP == Level->PortalPs[PortalIndex]){
				return PortalIndex + Tile_Type_PortalOne;
			}	
		}
	}
	return 0;
}

internal bool
CanSnakeBeTeleported(level *Level, uint32 SnakeID, uint32 PortalID){
	uint32 SnakeIndex = SnakeID - SNAKE_ID_OFFSET;
	assert(SnakeIndex >= 0 && SnakeIndex < Level->SnakeCount); 
	int32 PortalIndex = PortalID - Tile_Type_PortalOne;
	assert(PortalIndex >= 0 && PortalIndex <= 1);
	vec2i SourcePortalP = Level->PortalPs[PortalIndex];
	vec2i DestPortalP  = Level->PortalPs[(PortalIndex+1)%2];

	for(int32 p = 0; p < Level->Snakes[SnakeIndex].Length; p++){
		vec2i GridP = Level->Snakes[SnakeIndex].Parts[p].GridP;
		vec2i PartOffset = GridP - SourcePortalP;
		vec2i DestP = DestPortalP + PartOffset;
		if(	!IsPInBounds(DestP, Level->Width, Level->Height)  ||
			Level->Occupancy[DestP.X][DestP.Y] == Tile_Type_Solid ||
			Level->Occupancy[DestP.X][DestP.Y] == Tile_Type_Spikes||
			Level->Occupancy[DestP.X][DestP.Y] == Tile_Type_Fruit ||
			Level->Occupancy[DestP.X][DestP.Y] >= SNAKE_ID_OFFSET){
			return false;
		}
	}
	return true;
}

internal void
TeleportSnake(level *Level, uint32 SnakeID, uint32 SourcePortalID){
	uint32 SnakeIndex = SnakeID - SNAKE_ID_OFFSET;
	assert(SnakeIndex >= 0 && SnakeIndex < Level->SnakeCount);

	int32 PortalIndex = SourcePortalID - Tile_Type_PortalOne;
	assert(PortalIndex >= 0 && PortalIndex <= 1);
	vec2i SourcePortalP = Level->PortalPs[PortalIndex];
	vec2i DestPortalP  = Level->PortalPs[(PortalIndex+1)%2];

	for(int32 p = 0; p < Level->Snakes[SnakeIndex].Length; p++){
		vec2i GridP = Level->Snakes[SnakeIndex].Parts[p].GridP;
		Level->Occupancy[GridP.X][GridP.Y] = Tile_Type_Empty;

		vec2i PartOffset = GridP - SourcePortalP;
		vec2i DestP = DestPortalP + PartOffset;
		Level->Occupancy[DestP.X][DestP.Y] = SnakeID;
		Level->Snakes[SnakeIndex].Parts[p].GridP = DestP;
	}
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
		switch(Transition->Type){
			case Transition_Type_Slide:
			{
				Level->Occupancy[Tail->GridP.X][Tail->GridP.Y] = Tile_Type_Empty;
				for(uint32 i = Snake->Length-1; i > 0; i--){
					Snake->Parts[i].GridP = Snake->Parts[i-1].GridP;
				}
				Head->GridP = Transition->Slide.NewHeadP;
				Level->Occupancy[Head->GridP.X][Head->GridP.Y] = Snake->SnakeID;
			}
			break;
			case Transition_Type_Teleportation:
			{
				TeleportSnake(Level, Snake->SnakeID, Transition->Teleportation.SourcePortalID);
			}
			break;
			default:
			break;
		}
	}
}

internal void
UpdateLogic(game_state *GameState, game_input *Input, platform_services PlatformServices){
	if(Input->r.EndedDown && Input->r.Changed){
		ReloadLevel(GameState, PlatformServices);
	}
	if(!GameState->Player){
		return;
	}

	if(GameState->Player->SnakeID - SNAKE_ID_OFFSET >= GameState->Level.SnakeCount){
		if(GameState->Level.SnakeCount > 0){
			GameState->Player = &GameState->Level.Snakes[0];
		}else if(GameState->Level.SnakeCount == 0){
			GameState->Mode = Game_Mode_Overworld;
			return;
		}
	}

	for(uint32 i = 0; i < GameState->Level.SnakeCount; i++){
		if(GameState->Level.Snakes[i].Transition.Type != Transition_Type_None){
			GameState->t += 0.15f;
			if(GameState->t <= 1.0f){
				return;
			}else{
				GameState->t = 0.0f;
				GenerateNewStateAfterTransition(&GameState->Level, &GameState->MarkedSnakes);
				for(uint32 i = 0; i < GameState->Level.SnakeCount; i++){
					GameState->Level.Snakes[i].Transition = {};
				}
			}
			break;
		}
	}

	if(Input->Space.EndedDown && Input->Space.Changed){
		PlayNextSnake(GameState);
	}

	snake *Player = GameState->Player;
	level *Level = &GameState->Level;
	snake_part *Head = Player->Parts;

#if TELEPORTATION_MID_AIR 
	for(uint32 SnakeIndex = 0; SnakeIndex < Level->SnakeCount; SnakeIndex++){
		snake *Snake = Level->Snakes + SnakeIndex;
		uint32 PortalID  = IfOnPortalGetPortalID(Level, Snake->SnakeID);
		if(!Snake->IsOnPortal && PortalID && CanSnakeBeTeleported(Level, Snake->SnakeID, PortalID)){
			Snake->Transition.Type = Transition_Type_Teleportation;
			Snake->Transition.Teleportation.SourcePortalID = PortalID;
			Snake->IsOnPortal = true;
			return;
		}else if(!PortalID){
			Snake->IsOnPortal = false;
		}
	}
#endif

#if COMPLETE_AND_DELETE_MID_AIR 
	if(Level->FruitCount == 0){
		for(uint32 i = 0; i < Level->SnakeCount; i++){
			snake *Snake = Level->Snakes + i;
			if(Snake->Parts[0].GridP == Level->GoalP){
				DeleteSnakeReorderIDs(Level, Snake->SnakeID);	
				break;
			}
		}
	}
#endif

#if GRAVITY
	for(uint32 SnakeIndex = 0; SnakeIndex < Level->SnakeCount; SnakeIndex++){
		uint32 SnakeID = SnakeIndex + SNAKE_ID_OFFSET; 

		while(MarkIfSnakesCanMoove(Level, &GameState->MarkedSnakes, SnakeID, SNAKE_MAX_COUNT+SNAKE_ID_OFFSET, vec2i{0, -1})){
			if(WillAMarkedSnakeBePushedOnSpikes(Level, &GameState->MarkedSnakes, vec2i{0, -1})){
				ReloadLevel(GameState, PlatformServices);
				return;
			}
			PushMarkedSnakes(Level, &GameState->MarkedSnakes, vec2i{0, -1});

#if COMPLETE_AND_DELETE_MID_AIR 
			if(Level->FruitCount == 0){
				for(uint32 i = 0; i < Level->SnakeCount; i++){
					snake *Snake = Level->Snakes + i;
					if(Snake->Parts[0].GridP == Level->GoalP){
						DeleteSnakeReorderIDs(Level, Snake->SnakeID);	
						return;
					}
				}
			}
#endif
#if TELEPORTATION_MID_AIR
			for(int32 i = 0; i < GameState->MarkedSnakes.Count; i++){
				snake *Snake = GameState->MarkedSnakes.Snakes[i];
				uint32 PortalID  = IfOnPortalGetPortalID(Level, Snake->SnakeID);
				if(!Snake->IsOnPortal && PortalID && CanSnakeBeTeleported(Level, Snake->SnakeID, PortalID)){
					Snake->Transition.Type = Transition_Type_Teleportation;
					Snake->Transition.Teleportation.SourcePortalID = PortalID;
					Snake->IsOnPortal = true;
					return;
				}else if(!PortalID){
					Snake->IsOnPortal = false;
				}
			}
#endif
		}
	}
#endif

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
	
	vec2i NewP = Head->GridP + Direction;
	if(Direction != vec2i{} && IsInBounds(NewP.X, NewP.Y, Level->Width, Level->Height)){
		uint32 Value = Level->Occupancy[NewP.X][NewP.Y];

		if(	Value == Tile_Type_Empty || Value == Tile_Type_Goal ||
			Value == Tile_Type_PortalOne || Value == Tile_Type_PortalTwo){
			Player->Transition.Type = Transition_Type_Slide;
			Player->Transition.Slide.NewHeadP = NewP;
		}else if(Value >= SNAKE_ID_OFFSET && Value != Player->SnakeID && MarkIfSnakesCanMoove(Level, &GameState->MarkedSnakes, Value, Player->SnakeID, Direction)){
			for(int32 i = 0; i < GameState->MarkedSnakes.Count; i++){
				GameState->MarkedSnakes.Snakes[i]->Transition.Type = Transition_Type_GotPushed;
				GameState->MarkedSnakes.Snakes[i]->Transition.GotPushed.Direction = Direction;
			}
			Player->Transition.Type = Transition_Type_Slide;
			Player->Transition.Slide.NewHeadP = NewP;
		}else if (Value == Tile_Type_Fruit){
			Player->Length++;
			assert(Player->Length <= SNAKE_MAX_LENGTH);
			Level->FruitCount--;
			Player->Transition.Type = Transition_Type_Slide;
			Player->Transition.Slide.NewHeadP = NewP;
		}
	}
	if(IsPInBounds(Level->GoalP, Level->Width, Level->Height) && Level->Occupancy[Level->GoalP.X][Level->GoalP.Y] == Tile_Type_Empty){
		Level->Occupancy[Level->GoalP.X][Level->GoalP.Y] = Tile_Type_Goal;
	}
	if(IsPInBounds(Level->PortalPs[0], Level->Width, Level->Height) && Level->Occupancy[Level->PortalPs[0].X][Level->PortalPs[0].Y] == Tile_Type_Empty){
		Level->Occupancy[Level->PortalPs[0].X][Level->PortalPs[0].Y] = Tile_Type_PortalOne;
	}
	if(IsPInBounds(Level->PortalPs[1], Level->Width, Level->Height) && Level->Occupancy[Level->PortalPs[1].X][Level->PortalPs[1].Y] == Tile_Type_Empty){
		Level->Occupancy[Level->PortalPs[1].X][Level->PortalPs[1].Y] = Tile_Type_PortalTwo;
	}
}
#endif //GAMEPLAY_LOGIC_CPP
