#if !defined(LEVEL_EDITING_CPP)
#define LEVEL_EDITING_CPP

internal void
EditLevel(level *Level, rectangle ScreenOutline, uint32 *ActiveBrush, game_input *Input){
	vec2i MouseGridP = GetGridP(Input->MouseX, Input->MouseY, ScreenOutline);
	if(IsInBounds(MouseGridP.X, MouseGridP.Y, Level->Width, Level->Height)){
		if(Input->MouseLeft.EndedDown){
			if(*ActiveBrush >= SNAKE_ID_OFFSET){
				if(Input->MouseLeft.Changed){
					if(Level->Occupancy[MouseGridP.X][MouseGridP.Y] >= SNAKE_ID_OFFSET){
						DeleteSnakeReorderIDs(Level, Level->Occupancy[MouseGridP.X][MouseGridP.Y]);
					}
					snake NewSnake = snake{1, *ActiveBrush-SNAKE_ID_OFFSET};
					NewSnake.Parts[0].GridP = vec2i{MouseGridP.X, MouseGridP.Y};
					AddSnakeToLevel(Level, NewSnake);
					Level->Occupancy[MouseGridP.X][MouseGridP.Y] = Level->Snakes[Level->SnakeCount-1].SnakeID;
				}else{
					snake *LastAddedSnake = &Level->Snakes[Level->SnakeCount - 1];
					bool32 SelfIntersecting  = false;
					for(int32 p = 0; p < LastAddedSnake->Length; p++){
						vec2i GridP = LastAddedSnake->Parts[p].GridP;
						if(GridP.X == MouseGridP.X && GridP.Y == MouseGridP.Y){
							SelfIntersecting = true;
						}
					}
					if(!SelfIntersecting && LastAddedSnake->Length < SNAKE_MAX_LENGTH){
						if(Level->Occupancy[MouseGridP.X][MouseGridP.Y] >= SNAKE_ID_OFFSET){
							DeleteSnakeReorderIDs(Level, Level->Occupancy[MouseGridP.X][MouseGridP.Y]);	
						}
						LastAddedSnake = &Level->Snakes[Level->SnakeCount - 1];
						LastAddedSnake->Parts[LastAddedSnake->Length++].GridP = vec2i{MouseGridP.X, MouseGridP.Y};
						Level->Occupancy[MouseGridP.X][MouseGridP.Y] = Level->Snakes[Level->SnakeCount-1].SnakeID;
					}
				}
			}
			
			if(*ActiveBrush <= 4 && Level->Occupancy[MouseGridP.X][MouseGridP.Y] < SNAKE_ID_OFFSET){
				Level->Occupancy[MouseGridP.X][MouseGridP.Y] = *ActiveBrush;
			}
		}else if(Input->MouseRight.EndedDown){
			if(Level->Occupancy[MouseGridP.X][MouseGridP.Y] >= SNAKE_ID_OFFSET){
				DeleteSnakeReorderIDs(Level, Level->Occupancy[MouseGridP.X][MouseGridP.Y]);
			}else{
				Level->Occupancy[MouseGridP.X][MouseGridP.Y] = Tile_Type_Empty;
			}
		}
	}
	if(Input->Space.EndedDown && Input->Space.Changed){
		*ActiveBrush = ((*ActiveBrush)+1)%EDIT_BRUSH_COUNT ;
	}
}

internal void
TileLevel(game_state *GameState, rectangle ScreenOutline, rectangle GameBoardRect, game_input *Input){
	vec2i MouseGridP = GetGridP(Input->MouseX, Input->MouseY, GameBoardRect);
	if(Input->MouseLeft.EndedDown){
		if(GameState->SpriteAtlasActive && IsInsideRect(Input->MouseX, Input->MouseY, GameState->SpriteAtlasRect)){
			if(Input->MouseLeft.Changed){
				GameState->ActiveTileBrush = tile{GetTileGridRect(Input->MouseX, Input->MouseY, GameState->SpriteAtlasRect)};
			}
		}else if(IsPointInBounds(MouseGridP, GameState->Level.Width, GameState->Level.Height)){
			GameState->Level.Tiles[GameState->ActiveLayerIndex][MouseGridP.X][MouseGridP.Y] = GameState->ActiveTileBrush;
		}
	}else if(Input->MouseRight.EndedDown && IsPointInBounds(MouseGridP, GameState->Level.Width, GameState->Level.Height)){
		GameState->Level.Tiles[GameState->ActiveLayerIndex][MouseGridP.X][MouseGridP.Y] = {};
	}

	if(Input->Space.EndedDown && Input->Space.Changed){
		GameState->SpriteAtlasActive = !GameState->SpriteAtlasActive;
	}
	if(Input->ArrowLeft.EndedDown && Input->ArrowLeft.Changed){
		GameState->ActiveLayerIndex = CapInt32(0, GameState->ActiveLayerIndex - 1, LEVEL_MAX_LAYER_COUNT-1);
	}else if(Input->ArrowRight.EndedDown && Input->ArrowRight.Changed){
		GameState->ActiveLayerIndex = CapInt32(0, GameState->ActiveLayerIndex + 1, LEVEL_MAX_LAYER_COUNT-1);
	}
}

#endif //LEVEL_EDITING_CPP
