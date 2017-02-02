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
IsPInBounds(vec2i P, int32 MaxI, int32 MaxJ){
	return IsInBounds(P.X, P.Y, MaxI, MaxJ);
}

internal uint32
CapInt32(int32 a, int32 t, int32 b){
	if(t < a){
		return a;
	}else if (t > b){
		return b; }
	return t;
}

internal void
AddSnakeToLevel(level *Level, snake Snake){
	assert(Level->SnakeCount >= 0 && Level->SnakeCount < SNAKE_MAX_COUNT);
	Snake.SnakeID = Level->SnakeCount + SNAKE_ID_OFFSET;
	Level->Snakes[Level->SnakeCount++] = Snake;
}

internal animated_tile *
AddMobileTile(level *Level, vec3f WorldP, tile_brush *TileBrush){
	assert(Level->AnimatedTileCount >= 0 && Level->AnimatedTileCount < Level->AnimatedTileCapacity);
	return Level->AnimatedTiles + Level->AnimatedTileCount++;
}

internal void
AddTilemap(tilemap_palette *Palette, loaded_bitmap Bitmap, rectangle DestRect, int32 GridSpacingX, int32 GridSpacingY){
	assert(Palette->TilemapCount >= 0 && Palette->TilemapCount < Palette->TilemapCapacity);
	tilemap *New = &Palette->Tilemaps[Palette->TilemapCount++];
	New->Bitmap = Bitmap;
	New->DestRect = DestRect;
	New->GridSpacingX = GridSpacingX;
	New->GridSpacingY = GridSpacingY;
}

internal void
SwitchTilemap(tilemap_palette *Palette){
	Palette->TilemapIndex = (Palette->TilemapIndex+1)%Palette->TilemapCount;
}

internal rectangle
GetTilemapDestRect(tilemap_palette *Palette){
	return Palette->Tilemaps[Palette->TilemapIndex].DestRect;
}
internal loaded_bitmap*
GetTilemapBitmap(tilemap_palette *Palette){
	return &Palette->Tilemaps[Palette->TilemapIndex].Bitmap;
}

internal void
UpdateTilemapRects(tilemap_palette *Palette, int32 NewScreenHeight){
	for(int32 i = 0; i < Palette->TilemapCount; i++){
		tilemap *Tilemap = &Palette->Tilemaps[i];
		Tilemap->DestRect = {0, NewScreenHeight - Tilemap->Bitmap.Height, Tilemap->Bitmap.Width, NewScreenHeight};
	}
}

internal inline tilemap*
GetTilemap(tilemap_palette *Palette){
	return &Palette->Tilemaps[Palette->TilemapIndex];
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
		if(GameState->Level.GoalP == vec2i{}){
			GameState->Level.GoalP = vec2i{-1, -1};
		}
		if(GameState->Level.PortalPs[0] == vec2i{}){
			GameState->Level.PortalPs[0] = vec2i{-1, -1};
		}
		if(GameState->Level.PortalPs[1] == vec2i{}){
			GameState->Level.PortalPs[1] = vec2i{-1, -1};
		}
		if(GameState->Level.SnakeCount > 0){
			GameState->Player = &GameState->Level.Snakes[0];
		}else{
			GameState->Player = {};
		}
		GameState->Level.AnimatedTileCapacity = ANIMATED_TILE_MAX_COUNT;
	}else{
		GameState->Level = {}; 
		GameState->Player = {};
		GameState->Level.Width = LEVEL_MAX_WIDTH;
		GameState->Level.Height = LEVEL_MAX_HEIGHT;
	}
}

#endif //MISC_CPP
