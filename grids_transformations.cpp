#if !defined(GRID_RECT_CPP)
#define GRID_RECT_CPP

internal inline vec3f
GridPToWorldP(vec2f GridP, int32 LayerIndex, projection *Projection){
	vec3f WorldP = vec3f{GridP.X-0.5f*(real32)Projection->GridWidth,
						 GridP.Y-0.5f*(real32)Projection->GridHeight,
						 0.0005f*(real32)LayerIndex};
	return WorldP;
}

internal inline vec2i
WorldPToScreenP(vec3f WorldP, projection *Projection){
	vec3f CameraSpaceP = WorldP - Projection->CameraP;
	vec3f ProjectedP = CameraSpaceP/(-CameraSpaceP.Z*Projection->FocalLength);
	vec2i ScreenP = vec2i{(int32)(ProjectedP.X*Projection->UnitInPixels + 0.5f*(real32)Projection->ScreenWidthInPixels),
				(int32)((real32)Projection->ScreenHeightInPixels*0.5f-ProjectedP.Y*Projection->UnitInPixels)};
	return ScreenP;
}

internal rectangle
WorldRectToScreenRect(vec3f WorldP, real32 Width, real32 Height, projection *Projection){
	vec3f TopLeftWorldP 	= WorldP + vec3f{-0.0f, Height, 0.f};
	vec3f BottomRightWorldP 	= WorldP  + vec3f{Width, -0.0f, 0.f};
	vec2i MinScreenP = WorldPToScreenP(TopLeftWorldP, Projection);
	vec2i MaxScreenP = WorldPToScreenP(BottomRightWorldP, Projection);
	return rectangle{MinScreenP.X, MinScreenP.Y, MaxScreenP.X, MaxScreenP.Y};
}

internal rectangle
GridPToScreenRect(vec2f GridP, int32 LayerIndex, projection *Projection){
	vec3f CenterWorldP = GridPToWorldP(GridP, LayerIndex, Projection);
	return WorldRectToScreenRect(CenterWorldP, 1.0f, 1.0f, Projection);
}

#if 0
internal rectangle
GridPToScreenRect(vec2f GridP, int32 LayerIndex, projection *Projection){
	//Is assumed that rect width = height = 1(unit)
	vec3f CenterWorldP = GridPToWorldP(GridP, LayerIndex, Projection);
	vec3f TopLeftWorldP 	= CenterWorldP + vec3f{-0.0f, 1.0f, 0.f};
	vec3f BottomRightWorldP 	= CenterWorldP + vec3f{1.0f, -0.0f, 0.f};
	
	vec2i MinScreenP = WorldPToScreenP(TopLeftWorldP, Projection);
	vec2i MaxScreenP = WorldPToScreenP(BottomRightWorldP, Projection);
	return rectangle{MinScreenP.X, MinScreenP.Y, MaxScreenP.X, MaxScreenP.Y};
}
#endif

internal vec3f
ScreenPToWorldP(vec2f ScreenP, real32 WorldSpaceZ, projection *Projection){
	vec3f ProjectedP = {(ScreenP.X - 0.5f*(real32)Projection->ScreenWidthInPixels)/(real32)Projection->UnitInPixels,
						(0.5f*(real32)Projection->ScreenHeightInPixels - ScreenP.Y)/(real32)Projection->UnitInPixels, 0.0f};

	real32 CameraSpaceZ = WorldSpaceZ - Projection->CameraP.Z;
	vec3f CameraSpaceP = ProjectedP*(-CameraSpaceZ*Projection->FocalLength);
	CameraSpaceP.Z = CameraSpaceZ;
	vec3f WolrdP = CameraSpaceP + Projection->CameraP; 
	return WolrdP;
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
	if(IsPInBounds(Offset, GameRect.MaxX-GameRect.MinX, GameRect.MaxY-GameRect.MinY)){
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
GetTileGridP(int32 X, int32 Y, tilemap *Tilemap){
	return vec2i{(X-Tilemap->DestRect.MinX)/Tilemap->GridSpacingX, (Y-Tilemap->DestRect.MinY)/Tilemap->GridSpacingY};
}

internal inline rectangle
GetTileRect(int32 X, int32 Y, tilemap *Tilemap){
	vec2i GridP = GetTileGridP(X, Y, Tilemap);
	return rectangle{GridP.X*Tilemap->GridSpacingX,
					GridP.Y*Tilemap->GridSpacingY,
					(GridP.X+1)*Tilemap->GridSpacingX,
					(GridP.Y+1)*Tilemap->GridSpacingY};
}
#endif //GRID_RECT_CPP
