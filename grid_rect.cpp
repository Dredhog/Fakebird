#if !defined(GRID_RECT_CPP)
#define GRID_RECT_CPP

internal inline vec2f
WorldPToScreenP(vec3f WorldP, projection Projection){
	
	vec2f ScreenP = vec2f{WorldP.X, WorldP.Y}*Projection.UnitInPixels;
	return ScreenP;
}

internal inline vec3f
ScreenPToWorldP(vec2f ScreenP, real32 Z, projection Projection){
	vec3f WorldP = vec3f{ScreenP.X/Projection.UnitInPixels, ScreenP.Y/Projection.UnitInPixels, Z};
	return WorldP;
}

internal inline rectangle
LevelIndexToOverworldGridRect(int32 Index, rectangle GameRect){
	int32 RectsPerRow = (GameRect.MaxX - GameRect.MinX) / LEVEL_ICON_SIZE_IN_PIXELS;
	int32 Y = Index/RectsPerRow;
	int32 X = (Index - (Y*RectsPerRow)) % RectsPerRow;
	return rectangle{GameRect.MinX + X*LEVEL_ICON_SIZE_IN_PIXELS,
					GameRect.MinY + Y*LEVEL_ICON_SIZE_IN_PIXELS,
					GameRect.MinX + (X+1)*LEVEL_ICON_SIZE_IN_PIXELS,
					GameRect.MinY + (Y+1)*LEVEL_ICON_SIZE_IN_PIXELS};
}

internal inline int32
GetOverworldRectIndex(int32 MouseX, int32 MouseY, rectangle GameRect){
	int32 Result = -1;
	vec2i Offset = {MouseX - GameRect.MinX, MouseY - GameRect.MinY};
	if(IsPointInBounds(Offset, GameRect.MaxX-GameRect.MinX, GameRect.MaxY-GameRect.MinY)){
		int32 RectsPerWidth = (GameRect.MaxX- GameRect.MinX) / LEVEL_ICON_SIZE_IN_PIXELS;
		int32 Y = Offset.Y / LEVEL_ICON_SIZE_IN_PIXELS;
		int32 X = Offset.X / LEVEL_ICON_SIZE_IN_PIXELS;
		Result = RectsPerWidth*Y + X;
	}
	Result = (Result < 0  || Result >= LEVEL_MAX_COUNT) ? -1 : Result;
	return Result;
}

internal inline vec2i
GetGridP(int32 X, int32 Y, rectangle ScreenRect){
	return vec2i{(X-ScreenRect.MinX)/DEST_TILE_SIZE_IN_PIXELS, ((ScreenRect.MaxY - ScreenRect.MinY) - (Y-ScreenRect.MinY))/DEST_TILE_SIZE_IN_PIXELS};
}

internal inline rectangle
GetGridRect(int32 X, int32 Y, rectangle ScreenRect){
	return rectangle{X*DEST_TILE_SIZE_IN_PIXELS,
					ScreenRect.MaxY- (Y+1)*DEST_TILE_SIZE_IN_PIXELS,
					(X+1)*DEST_TILE_SIZE_IN_PIXELS,
					ScreenRect.MaxY - Y*DEST_TILE_SIZE_IN_PIXELS};
}

internal inline rectangle
GetGridRectFromScreenCoords(int32 X, int32 Y, rectangle ScreenRect){
	vec2i  GridP = GetGridP(X, Y, ScreenRect);
	return GetGridRect(GridP.X, GridP.Y, ScreenRect);
}

internal inline vec2i
GetTileGridP(int32 X, int32 Y, rectangle SprieAtlasRect){
	return vec2i{(X-SprieAtlasRect.MinX)/SOURCE_TILE_SIZE_IN_PIXELS, (Y-SprieAtlasRect.MinY)/SOURCE_TILE_SIZE_IN_PIXELS};
}

internal inline rectangle
GetTileGridRect(int32 X, int32 Y, rectangle SpriteAtlasRect){
	vec2i GridP = GetTileGridP(X, Y, SpriteAtlasRect);
	return rectangle{GridP.X*SOURCE_TILE_SIZE_IN_PIXELS,
					GridP.Y*SOURCE_TILE_SIZE_IN_PIXELS,
					(GridP.X+1)*SOURCE_TILE_SIZE_IN_PIXELS,
					(GridP.Y+1)*SOURCE_TILE_SIZE_IN_PIXELS};
}

internal rectangle
LerpGridRects(vec2i PosA, vec2i PosB, real32 t, rectangle ScreenRect){
	rectangle Result = GetGridRect(PosA.X, PosA.Y, ScreenRect);
	vec2i Delta = {(int32)(t*(real32)DEST_TILE_SIZE_IN_PIXELS*((real32)PosB.X - (real32)PosA.X)),
				   -(int32)(t*(real32)DEST_TILE_SIZE_IN_PIXELS*((real32)PosB.Y - (real32)PosA.Y))};
	Result.MinP += Delta;
	Result.MaxP += Delta;
	return Result;
}
#endif //GRID_RECT_CPP
