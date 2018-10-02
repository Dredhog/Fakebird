#if !defined(GRID_RECT_CPP)
#define GRID_RECT_CPP

internal inline vec3
GridPToWorldP(vec2f GridP, int32 LayerIndex, projection *Projection){
	vec3 WorldP = vec3{GridP.X-0.5f*(real32)Projection->GridWidth,
						 GridP.Y-0.5f*(real32)Projection->GridHeight,
						 0.0005f*(real32)LayerIndex};
	return WorldP;
}

internal inline vec2f
WorldPToScreenP(vec3 WorldP, projection *Projection){
	vec3 CameraSpaceP = WorldP - Projection->CameraP;
	vec3 ProjectedP = CameraSpaceP/(-CameraSpaceP.Z*Projection->FocalLength);
	vec2f ScreenP = {ProjectedP.X*Projection->UnitInPixels + 0.5f*(real32)Projection->ScreenWidthInPixels,
				(real32)Projection->ScreenHeightInPixels*0.5f-ProjectedP.Y*Projection->UnitInPixels};
	return ScreenP;
}

internal rectangle
WorldRectToScreenRect(vec3 WorldP, real32 Width, real32 Height, projection *Projection){
	vec3 TopLeftWorldP 	= WorldP + vec3{-0.0f, Height, 0.f};
	vec3 BottomRightWorldP 	= WorldP  + vec3{Width, -0.0f, 0.f};
	vec2f MinScreenP = WorldPToScreenP(TopLeftWorldP, Projection);
	vec2f MaxScreenP = WorldPToScreenP(BottomRightWorldP, Projection);
	return rectangle{MinScreenP.X, MinScreenP.Y, MaxScreenP.X, MaxScreenP.Y};
}

internal rectangle
GridPToScreenRect(vec2f GridP, int32 LayerIndex, projection *Projection){
	vec3 CenterWorldP = GridPToWorldP(GridP, LayerIndex, Projection);
	return WorldRectToScreenRect(CenterWorldP, 1.0f, 1.0f, Projection);
}


internal vec3
ScreenPToWorldP(vec2f ScreenP, real32 WorldSpaceZ, projection *Projection){
	vec3 ProjectedP = {(ScreenP.X - 0.5f*(real32)Projection->ScreenWidthInPixels)/(real32)Projection->UnitInPixels,
						(0.5f*(real32)Projection->ScreenHeightInPixels - ScreenP.Y)/(real32)Projection->UnitInPixels, 0.0f};

	real32 CameraSpaceZ = WorldSpaceZ - Projection->CameraP.Z;
	vec3 CameraSpaceP = ProjectedP*(-CameraSpaceZ*Projection->FocalLength);
	CameraSpaceP.Z = CameraSpaceZ;
	vec3 WolrdP = CameraSpaceP + Projection->CameraP; 
	return WolrdP;
}

internal inline rectangle
LevelIndexToOverworldGridRect(int32 Index, rectangle GameRect){
	int32 RectsPerRow = (int32)(GameRect.MaxX - GameRect.MinX) / LEVEL_ICON_SIZE_IN_PIXELS;
	real32 Y = (real32)(Index/RectsPerRow);
	real32 X = (real32)(((Index - ((int32)Y*RectsPerRow)) % RectsPerRow));
	return rectangle{GameRect.MinX + X*LEVEL_ICON_SIZE_IN_PIXELS,
					GameRect.MinY + Y*LEVEL_ICON_SIZE_IN_PIXELS,
					GameRect.MinX + (X+1)*LEVEL_ICON_SIZE_IN_PIXELS,
					GameRect.MinY + (Y+1)*LEVEL_ICON_SIZE_IN_PIXELS};
}

internal inline int32
GetOverworldRectIndex(int32 MouseX, int32 MouseY, rectangle GameRect){
	int32 Result = -1;
	vec2f Offset = {(real32)MouseX - GameRect.MinX, (real32)MouseY - GameRect.MinY};
	if(IsPInBounds(Offset, GameRect.MaxX-GameRect.MinX, GameRect.MaxY-GameRect.MinY)){
		int32 RectsPerWidth = (int32)(GameRect.MaxX- GameRect.MinX) / LEVEL_ICON_SIZE_IN_PIXELS;
		int32 Y = (int32)Offset.Y / LEVEL_ICON_SIZE_IN_PIXELS;
		int32 X = (int32)Offset.X / LEVEL_ICON_SIZE_IN_PIXELS;
		Result = RectsPerWidth*Y + X;
	}
	Result = (Result < 0  || Result >= LEVEL_MAX_COUNT) ? -1 : Result;
	return Result;
}

internal inline vec2i
GetGridP(int32 MouseX, int32 MouseY, rectangle ScreenRect){
	int32 ScreenMinX = (int32)ScreenRect.MinX;
	int32 ScreenMaxY = (int32)ScreenRect.MaxY;
	int32 ScreenMinY = (int32)ScreenRect.MinY;
	return {(MouseX-ScreenMinX)/(int32)DEST_TILE_SIZE_IN_PIXELS, ((ScreenMaxY  - ScreenMinY) - (MouseY-ScreenMinY ))/(int32)DEST_TILE_SIZE_IN_PIXELS};
}

internal inline rectangle
GetGridRect(int32 MouseX, int32 MouseY, rectangle ScreenRect){
	real32 X = (real32)MouseX;
	real32 Y = (real32)MouseY;
	return rectangle{X*DEST_TILE_SIZE_IN_PIXELS,
					ScreenRect.MaxY- (Y+1)*DEST_TILE_SIZE_IN_PIXELS,
					(X+1)*DEST_TILE_SIZE_IN_PIXELS,
					(ScreenRect.MaxY - Y*DEST_TILE_SIZE_IN_PIXELS)};
}

internal inline rectangle
GetGridRectFromScreenCoords(int32 X, int32 Y, rectangle ScreenRect){
	vec2i  GridP = GetGridP(X, Y, ScreenRect);
	return GetGridRect(GridP.X, GridP.Y, ScreenRect);
}

internal inline vec2i
GetTileGridP(int32 MouseX, int32 MouseY, tilemap *Tilemap){
	return vec2i{(MouseX-(int32)Tilemap->DestRect.MinX)/Tilemap->GridSpacingX, (MouseY-(int32)Tilemap->DestRect.MinY)/Tilemap->GridSpacingY};
}

internal inline rectangle
GetTileRect(int32 X, int32 Y, tilemap *Tilemap){
	vec2i GridP = GetTileGridP(X, Y, Tilemap);
	return rectangle{(real32)(GridP.X*Tilemap->GridSpacingX),
					(real32)(GridP.Y*Tilemap->GridSpacingY),
					((real32)GridP.X+1)*(real32)Tilemap->GridSpacingX,
					((real32)GridP.Y+1)*(real32)Tilemap->GridSpacingY};
}
#endif //GRID_RECT_CPP
