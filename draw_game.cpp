#if !defined(DRAW_GAME_CPP)
#define DRAW_GAME_CPP

static color SNAKE_COLORS[3][2] =	{{color{255, 10, 10, 255}, color{255, 76, 48, 255}},
									{color{0, 76, 255, 255}, color{0, 136, 255, 255}},
									{color{24, 209, 31, 255}, color{71, 219, 72, 255}}};
 
static color TILE_OUTLINE_COLOR = {255, 0, 255, 255};

static color TILE_COLORS[6] = {{154, 120, 55, 255},
							   {200, 200, 55, 255},
							   {154, 155, 155, 255},
							   {120, 50, 100, 255},
							   {255, 180, 200, 255},
							   {255, 200, 220, 255}};

static color TILEMAP_BACKGROUND_COLORS[LEVEL_MAX_LAYER_COUNT] = {	{24, 255, 31, 100}, 
																   	{200, 60, 255, 100},
																   	{200, 100, 40, 100}};
static color TILE_BRUSH_TYPE_COLORS[2] = {	{0, 0, 255, 150},
										{255, 255, 255, 150}};

internal void
DrawLevelTilesInLayerRange(level *Level, offscreen_buffer TargetBuffer, projection *Projection, int32 StartLayer, int32 EndLayer){
	for(int32 i = StartLayer; i < EndLayer; i++){
		for(uint32 j = 0; j < Level->Width; j++){
			for(uint32 k = 0; k < Level->Height; k++){
				rectangle DestRect = GridPToScreenRect(vec2f{(real32)j, (real32)k}, i, Projection);

				tile CurrentTile = Level->Tiles[i][j][k];
				if(CurrentTile.Bitmap){
					StretchBitmapOrthogonaly(TargetBuffer, *CurrentTile.Bitmap, DestRect, CurrentTile.SourceRect);
				}
			}
		}
	}
}

internal void
UpdateAnimatedTiles(level *Level){
	for(int32 i = 0; i < Level->AnimatedTileCount; i++){
		animated_tile *Tile = Level->AnimatedTiles + i;
		Tile->P += Tile->dP;
	}
}

internal void
DrawAnimatedTiles(level *Level, offscreen_buffer TargetBuffer, projection *Projection, game_input *Input, bool IsForeground){
	for(int32 i = 0; i < Level->AnimatedTileCount; i++){
		animated_tile *Tile = Level->AnimatedTiles + i;
		if((Tile->P.Z >= 0 && IsForeground) || (Tile->P.Z < 0 && !IsForeground)){
			rectangle DestRect = WorldRectToScreenRect(Tile->P, Tile->WidthInUnits, Tile->HeightInUnits, Projection);
			if(Tile->Bitmap){
				StretchBitmapOrthogonaly(TargetBuffer, *Tile->Bitmap, DestRect, Tile->SourceRect);
				if(IsInsideRect((real32)Input->MouseX, (real32)Input->MouseY, DestRect)){
					DrawRectOutline(TargetBuffer, DestRect, {0, 0, 0, 255});
				}
			}
		}
	}
}

internal void
DrawOverworld(overworld *Overworld, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline, rectangle GameBoardRect, int32 MouseX, int32 MouseY){
	rectangle DestRect;
	for(int32 i = 0; i < LEVEL_MAX_COUNT; i++){
			rectangle DestRect = LevelIndexToOverworldGridRect(i, GameBoardRect);
			color LevelIconColor = (Overworld->LevelInfos[i].Exists) ? color{150, 200, 120, 255} : color{200, 100, 70, 255};
			FillRect(OffscreenBuffer, DestRect, LevelIconColor);
			DrawRectOutline(OffscreenBuffer, DestRect, {150, 255, 80, 255});
	}
	int32 MouseRectIndex = GetOverworldRectIndex(MouseX, MouseY, GameBoardRect);
	if(MouseRectIndex >= 0){
		DestRect = LevelIndexToOverworldGridRect(MouseRectIndex, GameBoardRect);
		DrawRectOutline(OffscreenBuffer, DestRect, {100, 100, 200, 255});
	}
}

internal void
DrawEditModeLevelWithoutSolidAndSpikeBlocks(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	for(uint32 j = 0; j < GameState->Level.Width; j++){
		for(uint32 k = 0; k < GameState->Level.Height; k++){
			rectangle DestRect = GridPToScreenRect(vec2f{(real32)j, (real32)k}, 0, &GameState->Projection);

			uint32 Value = GameState->Level.Occupancy[j][k];
			if(Value <= Tile_Type_PortalTwo && Value > 1 && Value != 3){
				color TileColor = TILE_COLORS[Value - 1];

				FillRect(OffscreenBuffer, DestRect, TileColor);
				DrawRectOutline(OffscreenBuffer, DestRect, TILE_OUTLINE_COLOR);
			}
		}
	}
}

internal void
DrawEditModeLevel(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	for(uint32 j = 0; j < GameState->Level.Width; j++){
		for(uint32 k = 0; k < GameState->Level.Height; k++){
			rectangle DestRect = GetGridRect(j, k, ScreenOutline);

			uint32 Value = GameState->Level.Occupancy[j][k];
			if(Value <= Tile_Type_PortalTwo && Value > 0){
				color TileColor = TILE_COLORS[Value - 1];

				FillRect(OffscreenBuffer, DestRect, TileColor);
				DrawRectOutline(OffscreenBuffer, DestRect, TILE_OUTLINE_COLOR);
			}
		}
	}
}

internal void
DrawTileModeLevelAndUI(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	DrawEditModeLevel(GameState, OffscreenBuffer, ScreenOutline);
	for(int32 i = 0; i <= GameState->TileBrush.ActiveLayerIndex; i++){
		for(uint32 j = 0; j < GameState->Level.Width; j++){
			for(uint32 k = 0; k < GameState->Level.Height; k++){
				rectangle GridRect = GetGridRect(j, k, ScreenOutline);
				tile CurrentTile = GameState->Level.Tiles[i][j][k];
				if(CurrentTile.Bitmap){
					StretchBitmapOrthogonaly(OffscreenBuffer, *CurrentTile.Bitmap, GridRect, CurrentTile.SourceRect);
				}
			}
		}
	}
	if(GameState->TilemapPalette.Active){
		color Color = TILEMAP_BACKGROUND_COLORS[GameState->TileBrush.ActiveLayerIndex];
		rectangle TilemapRect = GetTilemapDestRect(&GameState->TilemapPalette);
		loaded_bitmap *TilemapBitmap = GetTilemapBitmap(&GameState->TilemapPalette);
		FillRect(OffscreenBuffer, TilemapRect, Color);
		StretchBitmapOrthogonaly(OffscreenBuffer, *TilemapBitmap, TilemapRect, rectangle{0, 0, (real32)TilemapBitmap->Width, (real32)TilemapBitmap->Height});
	}
}

internal void
DrawEditBrush(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline, int32 MouseX, int32 MouseY){
	rectangle MouseRect = GetGridRectFromScreenCoords(MouseX, MouseY, ScreenOutline);
	if(GameState->EditBrush <= Tile_Type_PortalTwo && GameState->EditBrush > 0){
		color TileColor = TILE_COLORS[GameState->EditBrush - 1];
		FillRect(OffscreenBuffer, MouseRect, TileColor);
		DrawRectOutline(OffscreenBuffer, MouseRect, TILE_OUTLINE_COLOR);
	}else if(GameState->EditBrush >= SNAKE_ID_OFFSET && GameState->EditBrush < SNAKE_ID_OFFSET + 3){
		color SnakeColor = SNAKE_COLORS[GameState->EditBrush-SNAKE_ID_OFFSET][0];
		FillRect(OffscreenBuffer, MouseRect, SnakeColor);
	}
}

internal void
DrawTileBrush(tile_brush *TileBrush, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline, int32 MouseX, int32 MouseY, projection *Projection){
	rectangle TileBrushRect = {};
	if(!TileBrush->IsOffGridMode){
		TileBrushRect = GetGridRectFromScreenCoords(MouseX, MouseY, ScreenOutline);
	}else{
		vec3 MouseWorldP = ScreenPToWorldP(vec2f{(real32)MouseX, (real32)MouseY}, TileBrush->WorldZ, Projection);
		TileBrushRect = WorldRectToScreenRect(MouseWorldP, TileBrush->WidthInUnits, TileBrush->HeightInUnits, Projection); 
	}

	if(TileBrush->Tile.Bitmap){
		StretchBitmapOrthogonaly(OffscreenBuffer, *TileBrush->Tile.Bitmap, TileBrushRect, TileBrush->Tile.SourceRect);
		if(TileBrush->IsOffGridMode){
			DrawRectOutline(OffscreenBuffer, TileBrushRect, TILE_BRUSH_TYPE_COLORS[TileBrush->Type]);
		}
	}
}

internal void
DrawSnakesInEditMode(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	for(uint32 SnakeIndex = 0; SnakeIndex < GameState->Level.SnakeCount; SnakeIndex++){
		snake *Snake = &GameState->Level.Snakes[SnakeIndex];
		for(int32 p = 0; p < Snake->Length; p++){
			vec2i PartPos =  Snake->Parts[p].GridP;
			rectangle DestRect = GetGridRect(PartPos.X, PartPos.Y, ScreenOutline);
			color Color = SNAKE_COLORS[Snake->PaletteIndex][p % 2];
			FillRect(OffscreenBuffer, DestRect, Color);
			if(p == 0 && Snake == GameState->Player){
				DrawRectOutline(OffscreenBuffer, DestRect, {200, 0, 255, 255});
			}
		}
	}
}

internal void
DrawSnakesWithPerspective(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	for(uint32 SnakeIndex = 0; SnakeIndex < GameState->Level.SnakeCount; SnakeIndex++){
		snake *Snake = &GameState->Level.Snakes[SnakeIndex];
		
		switch(Snake->Transition.Type){
			case Transition_Type_None:
			{
				for(int32 p = 0; p < Snake->Length; p++){
					vec2i PartPos =  Snake->Parts[p].GridP;
					rectangle DestRect = GridPToScreenRect(vec2f{(real32)PartPos.X, (real32)PartPos.Y}, 0, &GameState->Projection);
					color Color = SNAKE_COLORS[Snake->PaletteIndex][p % 2];
					FillRect(OffscreenBuffer, DestRect, Color);
					if(p == 0 && Snake == GameState->Player){
						DrawRectOutline(OffscreenBuffer, DestRect, {200, 0, 255, 255});
					}
				}
			}
			break;
			case  Transition_Type_Slide:
			{
				for(int32 p = Snake->Length-1; p > 0; p--){
					vec2f OldP =  Vec2f(Snake->Parts[p].GridP);
					vec2f NewP = Vec2f(Snake->Parts[p-1].GridP);
					vec2f LerpedP = OldP*(1.0f - GameState->t) + NewP*GameState->t;
					rectangle DestRect = GridPToScreenRect(LerpedP, 0, &GameState->Projection);
					color Color = SNAKE_COLORS[Snake->PaletteIndex][p % 2];
					FillRect(OffscreenBuffer, DestRect, Color);
				}
				vec2f OldP =  Vec2f(Snake->Parts[0].GridP);
				vec2f NewP = Vec2f(Snake->Transition.Slide.NewHeadP);
				vec2f LerpedP = OldP*(1.0f - GameState->t) + NewP*GameState->t;
				rectangle DestRect = GridPToScreenRect(LerpedP, 0, &GameState->Projection);
				color Color = SNAKE_COLORS[Snake->PaletteIndex][0];
				FillRect(OffscreenBuffer, DestRect, Color);
				if(Snake == GameState->Player){
					DrawRectOutline(OffscreenBuffer, DestRect, {200, 0, 255, 255});
				}
			}
			break;
			case  Transition_Type_GotPushed:
			{
				for(int32 p = Snake->Length-1; p >= 0; p--){
					vec2f OldP = Vec2f(Snake->Parts[p].GridP);
					vec2f NewP = OldP + Vec2f(Snake->Transition.GotPushed.Direction);
					vec2f LerpedP = OldP*(1.0f - GameState->t) + NewP*GameState->t;
					rectangle DestRect = GridPToScreenRect(LerpedP, 0, &GameState->Projection);
					color Color = SNAKE_COLORS[Snake->PaletteIndex][p % 2];
					FillRect(OffscreenBuffer, DestRect, Color);
				}
			}
			break;
			case Transition_Type_Teleportation:
			{
				for(int32 p = 0; p < Snake->Length; p++){
					vec2i PartPos =  Snake->Parts[p].GridP;
					rectangle DestRect = GetGridRect(PartPos.X, PartPos.Y, ScreenOutline);
					color Color = SNAKE_COLORS[Snake->PaletteIndex][p % 2];
					Color.RGBA.A = (uint8)(255.0f * (1.0f-GameState->t));
					FillRect(OffscreenBuffer, DestRect, Color);
					if(p == 0 && Snake == GameState->Player){
						DrawRectOutline(OffscreenBuffer, DestRect, {200, 0, 255, 255});
					}
				}
			}
			break;
			default:
			{
			}
			break;
		}
	}
}

#endif //DRAW_GAME_CPP
