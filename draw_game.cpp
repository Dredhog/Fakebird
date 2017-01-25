#if !defined(DRAW_GAME_CPP)
#define DRAW_GAME_CPP

static color SNAKE_COLORS[3][2] =      {{color{255, 10, 10, 255}, color{255, 76, 48, 255}},
									   {color{0, 76, 255, 255}, color{0, 136, 255, 255}},
									   {color{24, 209, 31, 255}, color{71, 219, 72, 255}}};
 
static color TILE_OUTLINE_COLOR = {200, 150, 55, 100};

static color TILE_COLORS[5] = {{154, 120, 55, 255},
							   {200, 200, 55, 255},
							   {154, 155, 155, 255},
							   {120, 50, 100, 255},
							   {255, 50, 200, 255}};

static color TILEMAP_BACKGROUND_COLORS[LEVEL_MAX_LAYER_COUNT] = {	{24, 255, 31, 150}, 
																   	{200, 60, 255, 150},
																   	{200, 100, 40, 150}};

internal void
DrawPlayModeElements(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	for(uint32 j = 0; j < GameState->Level.Width; j++){
		for(uint32 k = 0; k < GameState->Level.Height; k++){
			rectangle DestRect = GetGridRect(j, k, ScreenOutline);

			uint32 Value = GameState->Level.Occupancy[j][k];
			if(Value <= 4 && Value > 1 && Value != 3){
				color TileColor;
				if(Value == 4){
				 	TileColor = (GameState->FruitRemaining == 0) ? TILE_COLORS[Tile_Type_Goal] : TILE_COLORS[Tile_Type_Goal-1];
				}else{
					TileColor = TILE_COLORS[Value - 1];
				}

				FillRect(OffscreenBuffer, DestRect, TileColor);
				DrawRectOutline(OffscreenBuffer, DestRect, TILE_OUTLINE_COLOR);
			}
		}
	}
}

internal void
DrawPlayModeLevel(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline, int32 StartLayer, int32 EndLayer){
	for(int32 i = StartLayer; i < EndLayer; i++){
		for(uint32 j = 0; j < GameState->Level.Width; j++){
			for(uint32 k = 0; k < GameState->Level.Height; k++){
				rectangle GridRect = GetGridRect(j, k, ScreenOutline);
				StretchBitmapOrthogonaly(OffscreenBuffer, GameState->SpriteAtlas, GridRect, GameState->Level.Tiles[i][j][k].Source);
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
			if(Value <= 4 && Value > 0){
				color TileColor = TILE_COLORS[Value - 1];
				if(Value == 4){
					 TileColor = (GameState->FruitRemaining == 0) ? TILE_COLORS[Tile_Type_Goal] : TILE_COLORS[Tile_Type_Goal-1];
				}

				FillRect(OffscreenBuffer, DestRect, TileColor);
				DrawRectOutline(OffscreenBuffer, DestRect, TILE_OUTLINE_COLOR);
			}
		}
	}
}

internal void
DrawTileModeLevelAndUI(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	for(uint32 j = 0; j < GameState->Level.Width; j++){
		for(uint32 k = 0; k < GameState->Level.Height; k++){
			rectangle DestRect = GetGridRect(j, k, ScreenOutline);

			uint32 Value = GameState->Level.Occupancy[j][k];
			if(Value <= 4 && Value > 0){
				color TileColor = TILE_COLORS[Value - 1];
				FillRect(OffscreenBuffer, DestRect, TileColor);
				DrawRectOutline(OffscreenBuffer, DestRect, TILE_OUTLINE_COLOR);
			}
		}
	}
	for(int32 i = 0; i <= GameState->ActiveLayerIndex; i++){
		for(uint32 j = 0; j < GameState->Level.Width; j++){
			for(uint32 k = 0; k < GameState->Level.Height; k++){
				rectangle GridRect = GetGridRect(j, k, ScreenOutline);
				StretchBitmapOrthogonaly(OffscreenBuffer, GameState->SpriteAtlas, GridRect, GameState->Level.Tiles[i][j][k].Source);
			}
		}
	}
	if(GameState->SpriteAtlasActive){
		color Color = TILEMAP_BACKGROUND_COLORS[GameState->ActiveLayerIndex];
		FillRect(OffscreenBuffer, GameState->SpriteAtlasRect, Color);
		StretchBitmapOrthogonaly(OffscreenBuffer, GameState->SpriteAtlas, GameState->SpriteAtlasRect, rectangle{0, 0, GameState->SpriteAtlas.Width, GameState->SpriteAtlas.Height});
	}
}

internal void
DrawEditBrush(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline, int32 MouseX, int32 MouseY){
	rectangle MouseRect = GetGridRectFromScreenCoords(MouseX, MouseY, ScreenOutline);
	if(GameState->ActiveBrush <= 4 && GameState->ActiveBrush > 0){
		color TileColor = TILE_COLORS[GameState->ActiveBrush - 1];
		FillRect(OffscreenBuffer, MouseRect, TileColor);
		DrawRectOutline(OffscreenBuffer, MouseRect, TILE_OUTLINE_COLOR);
	}else if(GameState->ActiveBrush >= SNAKE_ID_OFFSET && GameState->ActiveBrush < SNAKE_ID_OFFSET + 3){
		color SnakeColor = SNAKE_COLORS[GameState->ActiveBrush-SNAKE_ID_OFFSET][0];
		FillRect(OffscreenBuffer, MouseRect, SnakeColor);
	}
}

internal void
DrawTileBrush(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline, int32 MouseX, int32 MouseY){
	rectangle TileBrushRect = GetGridRectFromScreenCoords(MouseX, MouseY, ScreenOutline);
	StretchBitmapOrthogonaly(OffscreenBuffer, GameState->SpriteAtlas, TileBrushRect, GameState->ActiveTileBrush.Source);
}

internal void
DrawSnakes(game_state *GameState, offscreen_buffer OffscreenBuffer, rectangle ScreenOutline){
	for(uint32 SnakeIndex = 0; SnakeIndex  < GameState->Level.SnakeCount; SnakeIndex++){
		snake *Snake = &GameState->Level.Snakes[SnakeIndex];
		for(uint32 p = 0; p < Snake->Length; p++){
			vec2i PartPos =  Snake->Parts[p].GridP;

			rectangle DestRect = GetGridRect(PartPos.X, PartPos.Y, ScreenOutline);
			color Color = SNAKE_COLORS[Snake->PaletteIndex][p % 2];
			FillRect(OffscreenBuffer, DestRect, Color);
		}
	}
}
#endif //DRAW_GAME_CPP
