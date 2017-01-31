#if !defined(MISC_CPP)
#define MISC_CPP

internal inline bool32
IsInBounds(int32 i, int32 j, int32 MaxI, int32 MaxJ){
	if(i < 0 || j < 0 || i >= MaxI || j >= MaxJ){
		return false;
	}	
	return true;
}

internal inline bool32
IsPointInBounds(vec2i P, int32 MaxI, int32 MaxJ){
	return IsInBounds(P.X, P.Y, MaxI, MaxJ);
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
AddSnakeToLevel(level *Level, snake Snake){
	assert(Level->SnakeCount >= 0 && Level->SnakeCount < SNAKE_MAX_COUNT);
	Snake.SnakeID = Level->SnakeCount + SNAKE_ID_OFFSET;
	Level->Snakes[Level->SnakeCount++] = Snake;
}

internal void
AddBitmap(game_state *GameState, loaded_bitmap Bitmap){
	assert(GameState->BitmapCount >= 0 && GameState->BitmapCount < BITMAP_MAX_COUNT);
	GameState->Bitmaps[GameState->BitmapCount++] = Bitmap;
}

internal void
DeleteSnakeReorderIDs(level *Level, uint32 SnakeID){
	uint32 RemoveIndex = SnakeID - SNAKE_ID_OFFSET;
	assert(RemoveIndex >= 0 && RemoveIndex < Level->SnakeCount);

	for(int32 p = 0; p < Level->Snakes[RemoveIndex].Length; p++){
		vec2i PartP = Level->Snakes[RemoveIndex].Parts[p].GridP;
		Level->Occupancy[PartP.X][PartP.Y] = Tile_Type_Empty;
	}
	for(uint32 i = RemoveIndex; i < Level->SnakeCount-1; i++){
		Level->Snakes[i] = Level->Snakes[i+1];
	}
	Level->SnakeCount--;

	for(uint32 i = RemoveIndex; i < Level->SnakeCount; i++){
		Level->Snakes[i].SnakeID = i + SNAKE_ID_OFFSET;
		for(int32 p = 0; p < Level->Snakes[i].Length; p++){
			vec2i PartP = Level->Snakes[i].Parts[p].GridP;
			Level->Occupancy[PartP.X][PartP.Y] = Level->Snakes[i].SnakeID;
		}
	}
}

internal void
ReloadLevel(game_state *GameState, platform_services PlatformServices)
{
	assert(GameState->LevelIndex >= 0 && GameState->LevelIndex <= LEVEL_MAX_COUNT);
	debug_read_file_result LevelHandle {};
	GameState->Overworld.ActiveLevelName[0] = (char)((GameState->LevelIndex/10)+48);
	GameState->Overworld.ActiveLevelName[1] = (char)((GameState->LevelIndex%10)+48);
	GameState->Overworld.ActiveLevelName[2] = '\0';
	if(GameState->Overworld.LevelInfos[GameState->LevelIndex].Exists){
		LevelHandle = PlatformServices.ReadEntireFile(GameState->Overworld.ActiveLevelName);
	}
	if(LevelHandle.ContentsSize){
		memcpy(&GameState->Level, LevelHandle.Contents, LevelHandle.ContentsSize);
		PlatformServices.FreeFileMemory(LevelHandle.Contents);
		GameState->Level.FruitCount = 0;
		for(uint32 i = 0; i < GameState->Level.Width; i++){
			for(uint32 j = 0; j < GameState->Level.Height; j++){
				uint32 Value = GameState->Level.Occupancy[i][j];
				if(Value == 2){
					GameState->Level.FruitCount++;
				}else if(Value == 4){
					GameState->Level.GoalP = vec2i{(int32)i, (int32)j};
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
}

#endif //MISC_CPP
