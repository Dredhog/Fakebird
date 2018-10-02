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
					snake NewSnake = {};
					NewSnake.Length = 1;
					NewSnake.PaletteIndex = *ActiveBrush-SNAKE_ID_OFFSET;
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
			}else if(Level->Occupancy[MouseGridP.X][MouseGridP.Y] < SNAKE_ID_OFFSET){
				if(*ActiveBrush <= Tile_Type_Goal){
					Level->Occupancy[MouseGridP.X][MouseGridP.Y] = *ActiveBrush;
				}else if(Input->MouseLeft.Changed &&
						(*ActiveBrush == Tile_Type_PortalOne || *ActiveBrush == Tile_Type_PortalTwo)){
					vec2i OldPortalGridP = Level->PortalPs[*ActiveBrush - Tile_Type_PortalOne];
					Level->Occupancy[OldPortalGridP.X][OldPortalGridP.Y] = Tile_Type_Empty;
					Level->Occupancy[MouseGridP.X][MouseGridP.Y] = *ActiveBrush;
					Level->PortalPs[*ActiveBrush - Tile_Type_PortalOne] = MouseGridP;
				}
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
TileLevel(level *Level, tilemap_palette *Palette, tile_brush *TileBrush, rectangle ScreenOutline, rectangle GameBoardRect, game_input *Input, projection *Projection){
	vec2i MouseGridP = GetGridP(Input->MouseX, Input->MouseY, GameBoardRect);

	if(Palette->Active && Input->MouseLeft.EndedDown && Input->MouseLeft.Changed){
		if(IsInsideRect((real32)Input->MouseX, (real32)Input->MouseY, GetTilemapDestRect(Palette))){
			tilemap *Tilemap = GetTilemap(Palette);
			TileBrush->Tile = tile{&Tilemap->Bitmap, GetTileRect(Input->MouseX, Input->MouseY, Tilemap)};
			TileBrush->WidthInUnits = (real32)Tilemap->GridSpacingX / Projection->UnitInPixels;
			TileBrush->HeightInUnits = (real32)Tilemap->GridSpacingY / Projection->UnitInPixels;
		}
	}
	if(!TileBrush->IsOffGridMode){
		if(Input->MouseLeft.EndedDown){
			if(!(Palette->Active && IsInsideRect((real32)Input->MouseX, (real32)Input->MouseY, GetTilemapDestRect(Palette)))){
				Level->Tiles[TileBrush->ActiveLayerIndex][MouseGridP.X][MouseGridP.Y] = TileBrush->Tile;
			}
		} else if(Input->MouseRight.EndedDown && IsPInBounds(MouseGridP, Level->Width, Level->Height)){
			Level->Tiles[TileBrush->ActiveLayerIndex][MouseGridP.X][MouseGridP.Y] = {};
		}
	}else{
		if(!(Palette->Active  && IsInsideRect((real32)Input->MouseX, (real32)Input->MouseY, GetTilemapDestRect(Palette)))){
			if(Input->MouseLeft.EndedDown && Input->MouseLeft.Changed){
				vec3 MouseWorldP = ScreenPToWorldP(vec2f{(real32)Input->MouseX, (real32)Input->MouseY}, TileBrush->WorldZ, Projection);
				animated_tile *Tile = AddAnimatedTile(Level, MouseWorldP, TileBrush);
				*Tile = {};
				Tile->Bitmap = TileBrush->Tile.Bitmap;
				Tile->SourceRect = TileBrush->Tile.SourceRect;
				Tile->P = MouseWorldP;
				Tile->WidthInUnits = TileBrush->WidthInUnits;
				Tile->HeightInUnits = TileBrush->HeightInUnits;
				if(TileBrush->Type == TileBrush_Mobile){
					Tile->dP = {0.01f, 0.0f, 0.0f};
				}else if(TileBrush->Type == TileBrush_Static){
					Tile->dP = {0.0f, 0.0f, 0.0f};
				}
			}else if(Input->MouseRight.EndedDown && Input->MouseRight.Changed){
				DeleteSelectedAnimatedTile(Level, Input->MouseX, Input->MouseY, Projection);
			}
		}
		if(Input->ArrowUp.EndedDown){
			TileBrush->WorldZ -= 0.01f;
		}else if(Input->ArrowDown.EndedDown){
			TileBrush->WorldZ += 0.01f;
		}
	}

	if(Input->n.EndedDown && Input->n.Changed){
		SwitchTilemap(Palette);
	}
	if(Input->Space.EndedDown && Input->Space.Changed){
		Palette->Active = !Palette->Active;
	}
	if(Input->ArrowLeft.EndedDown && Input->ArrowLeft.Changed){
		TileBrush->ActiveLayerIndex = CapInt32(0, TileBrush->ActiveLayerIndex - 1, LEVEL_MAX_LAYER_COUNT-1);
	}else if(Input->ArrowRight.EndedDown && Input->ArrowRight.Changed){
		TileBrush->ActiveLayerIndex = CapInt32(0, TileBrush->ActiveLayerIndex + 1, LEVEL_MAX_LAYER_COUNT-1);
	}

	if(Input->m.EndedDown && Input->m.Changed){
		TileBrush->Type = (TileBrush->Type == TileBrush_Mobile) ? TileBrush_Static : TileBrush_Mobile;
	}
	if(Input->g.EndedDown && Input->g.Changed){
		TileBrush->IsOffGridMode = !TileBrush->IsOffGridMode;
		TileBrush->Type = TileBrush_Mobile;
	}
}

#endif //LEVEL_EDITING_CPP
