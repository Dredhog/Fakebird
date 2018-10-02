#if !defined(MISC_CPP)
#define MISC_CPP

internal void
AddSnakeToLevel(level *Level, snake Snake){
	assert(Level->SnakeCount >= 0 && Level->SnakeCount < SNAKE_MAX_COUNT);
	Snake.SnakeID = Level->SnakeCount + SNAKE_ID_OFFSET;
	Level->Snakes[Level->SnakeCount++] = Snake;
}

internal animated_tile *
AddAnimatedTile(level *Level, vec3 WorldP, tile_brush *TileBrush){
	assert(Level->AnimatedTileCount >= 0 && Level->AnimatedTileCount < Level->AnimatedTileCapacity);
	//Level->AnimatedTiles[AnimatedTIleCount] = 
	return Level->AnimatedTiles + Level->AnimatedTileCount++;
}

internal void
DeleteAnimatedTile(level *Level, int32 Index){
	assert(Index >= 0 && Index < Level->AnimatedTileCount);
	assert(Level->AnimatedTileCount > 0);
	if(Level->AnimatedTileCount > 1){
		Level->AnimatedTiles[Index] = Level->AnimatedTiles[--Level->AnimatedTileCount];
	}else{
		Level->AnimatedTiles[Index] = {};
		--Level->AnimatedTileCount;
	}
}

internal void
DeleteSelectedAnimatedTile(level *Level, int32 MouseX, int32 MouseY, projection *Projection){
	for(int32 i = 0; i < Level->AnimatedTileCount; i++){
		animated_tile *Tile = &Level->AnimatedTiles[i];
		rectangle TileScreenRect = WorldRectToScreenRect(Tile->P, Tile->WidthInUnits,
													Tile->HeightInUnits, Projection);
		if(IsInsideRect((real32)MouseX, (real32)MouseY, TileScreenRect)){
			DeleteAnimatedTile(Level, i);
			i--;
		}
	}
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
		Tilemap->DestRect = {0, (real32)(NewScreenHeight - Tilemap->Bitmap.Height), (real32)Tilemap->Bitmap.Width, (real32)NewScreenHeight};
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
		PlatformServices.FreeFileMemory(LevelHandle);
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
		GameState->Level.AnimatedTileCapacity = ANIMATED_TILE_MAX_COUNT;
	}
}
#endif //MISC_CPP
