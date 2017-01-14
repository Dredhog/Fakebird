#if !defined(RENDERER_CPP)
#define RENDERER_CPP

internal void 
ClearOffscreenBuffer(offscreen_buffer Buffer, color FillColor){
	FillColor = color{FillColor.R, FillColor.G, FillColor.B, FillColor.A};
	for(int32 Y = 0; Y < Buffer.Height; Y++){
		color *Pixels = (color*)((uint8*)Buffer.Memory + Y*Buffer.Pitch);
		for(int32 X = 0; X < Buffer.Width; X++){
			*Pixels = FillColor;
			Pixels++;
		}
	}
}

internal inline int32
MinInt32(int32 a, int32 b){
	if(a < b){
		return a;
	}
	return b;
}

internal inline int32
MaxInt32(int32 a, int32 b){
	if(a > b){
		return a;
	}
	return b;
}

internal bool
IsInsideRect(int32 X, int32 Y, rectangle Rect){
	if(X >= Rect.X && X < Rect.X+Rect.W && Y >= Rect.Y && Y < Rect.Y+Rect.H){
		return true;
	}
	return false;
}

internal bool
IsPointInsideRect(vec2i P, rectangle Rect){
	return IsInsideRect(P.X, P.Y, Rect);
}

internal void
FillRect(offscreen_buffer Buffer, rectangle Rect, color FillColor){
	FillColor = color{FillColor.R, FillColor.G, FillColor.B, FillColor.A};
	int32 MinX = MaxInt32(Rect.X, 0);
	int32 MinY = MaxInt32(Rect.Y, 0);
	int32 MaxX = MinInt32(Rect.X+Rect.W, (int32)Buffer.Width);
	int32 MaxY = MinInt32(Rect.Y+Rect.H, (int32)Buffer.Height);

	for(int32 Y = MinY; Y < MaxY; Y++){
		color *Pixels = (color*)((uint8*)Buffer.Memory + Y*Buffer.Pitch);
		for(int32 X = MinX; X < MaxX; X++){
			*(Pixels+X) = FillColor;
		}
	}
}

internal void
DrawRectOutline(offscreen_buffer Buffer, rectangle Rect, color FillColor){
	FillColor = color{FillColor.R, FillColor.G, FillColor.B, FillColor.A};
	//top line
	if(Rect.Y >= 0 && Rect.Y < Buffer.Height){
		int32 MinX = MaxInt32(Rect.X, 0);
		int32 MaxX = MinInt32(Rect.X+Rect.W, Buffer.Width);

		color *Color = (color*)Buffer.Memory + Rect.Y*Buffer.Width;
		for(int32 X = MinX; X < MaxX; X++){
			*(Color + X) = FillColor;
		}
	}
	//bottom line
	if(Rect.Y+Rect.H-1 >= 0 && Rect.Y+Rect.H-1 < Buffer.Height){
		int32 MinX = MaxInt32(Rect.X, 0);
		int32 MaxX = MinInt32(Rect.X+Rect.W, Buffer.Width);

		color *Color = (color*)Buffer.Memory + (Rect.Y+Rect.H-1)*Buffer.Width;
		for(int32 X = MinX; X < MaxX; X++){
			*(Color + X) = FillColor;
		}

	}
	//left line
	if(Rect.X >= 0 && Rect.X < Buffer.Width){
		int32 MinY = MaxInt32(Rect.Y, 0);
		int32 MaxY = MinInt32(Rect.Y+Rect.H, Buffer.Height);

		color *Color = (color*)Buffer.Memory + Rect.X;
		for(int32 Y = MinY; Y < MaxY; Y++){
			*(Color + Y*Buffer.Width) = FillColor;
		}
	}
	//right line
	if(Rect.X+Rect.W-1 >= 0 && Rect.X+Rect.W-1 < Buffer.Width){
		int32 MinY = MaxInt32(Rect.Y, 0);
		int32 MaxY = MinInt32(Rect.Y+Rect.H, Buffer.Height);

		color *Color = (color*)Buffer.Memory + (Rect.X+Rect.W-1);
		for(int32 Y = MinY; Y < MaxY; Y++){
			*(Color + Y*Buffer.Width) = FillColor;
		}
	}
}
#if 0
internal void
BlitBitmap(offscreen_buffer Buffer, vec2i Dest, loaded_bitmap Bitmap, rectangle Src){
	assert(Src.W >= 0 || Src.H >= 0);

	int32 SrcMinX = (Src.X >= 0) ? Src.X : 0;
	int32 SrcMinY = (Src.Y >= 0) ? Src.Y : 0;
	int32 SrcMaxX = (Src.X+Src.W < (int32)Bitmap.Width) ? Src.X+Src.W : Bitmap.Width;
	int32 SrcMaxY = (Src.Y+Src.H < (int32)Bitmap.Height) ? Src.Y+Src.H : Bitmap.Height;

	int32 DestMinX = (Dest.X >= 0) ? Dest.X : 0;
	int32 DestMinY = (Dest.Y >= 0) ? Dest.Y : 0;
	int32 DestMaxX = (Dest.X+Src.W < (int32)Buffer.Width) ? Dest.X+Src.W : Buffer.Width;
	int32 DestMaxY = (Dest.Y+Src.H < (int32)Buffer.Height) ? Dest.Y+Src.H : Buffer.Height;

	int32 CopyHeight = MinInt32(SrcMaxY-SrcMinY, DestMaxY-DestMinY);
	int32 CopyWidth = MinInt32(SrcMaxX-SrcMinX, DestMaxX-DestMinX);

	if(CopyWidth > 0){
		for(int32 Y = 0; Y < CopyHeight; Y++){
			memcpy((color*)Buffer.Memory + (DestMinY + Y)*Buffer.Width + DestMinX, (color*)Bitmap.Texels + (SrcMinY + Y)*Bitmap.Width + SrcMinX, CopyWidth*sizeof(color));
		}
	}
}
#endif

internal void
StretchBitmapOrthogonaly(offscreen_buffer Buffer, rectangle Dest, loaded_bitmap Bitmap, rectangle Source){
	assert(Source.W >= 0);
	assert(Source.H >= 0);
	assert(Dest.W >= 0);
	assert(Dest.H >= 0);
	if(Source.W <= 0 || Source.H <= 0){
		return;
	}

	real32 	ScaleX = ((real32)Source.W)/((real32)Dest.W);
	real32 	ScaleY = ((real32)Source.H)/((real32)Dest.H);
	vec2i  	DestMinCorner 	= {MaxInt32(Dest.X, 0), MaxInt32(Dest.Y, 0)};
	vec2i	DestMaxCorner 		= {MinInt32(Dest.X+Dest.W, Buffer.Width), MinInt32(Dest.Y+Dest.H, Buffer.Height)};

	vec2i	DestRelativeMinCorner = DestMinCorner - vec2i{Dest.X, Dest.Y};
	vec2i	DestRelativeMaxCorner  = DestMaxCorner - vec2i{Dest.X, Dest.Y};

	rectangle ClippedSourceRect = {MaxInt32(Source.X, 0), MaxInt32(Source.Y, 0)};
	ClippedSourceRect.W = MinInt32(Source.X+Source.W, Bitmap.Width) - ClippedSourceRect.X;
	ClippedSourceRect.H = MinInt32(Source.Y+Source.H, Bitmap.Height) - ClippedSourceRect.Y;
	assert(ClippedSourceRect.W > 0 && ClippedSourceRect.H > 0);

	color *DestStart   = ((color*)Buffer.Memory) + Dest.Y*Buffer.Width + Dest.X;
	color *SourceStart   = ((color*)Bitmap.Texels) + Source.Y*Bitmap.Width + Source.X;
	for(int32 Y = DestRelativeMinCorner.Y; Y < DestRelativeMaxCorner.Y; Y++){
		for(int32 X = DestRelativeMinCorner.X; X < DestRelativeMaxCorner.X; X++){
			color *DestPixel = DestStart + Y*Buffer.Width + X;
			vec2i SourceP = {(int32)(ScaleX*(real32)X), (int32)(ScaleY*(real32)Y)};
			if(IsPointInsideRect(SourceP+vec2i{Source.X, Source.Y}, ClippedSourceRect)){
				color *SourcePixel = SourceStart + SourceP.Y*Bitmap.Width + SourceP.X;

				real32 t = ((real32)SourcePixel->A)/255.0f;
				DestPixel->R = (uint8)(DestPixel->R*(1.0f-t) + SourcePixel->R*t);
				DestPixel->G = (uint8)(DestPixel->G*(1.0f-t) + SourcePixel->G*t);
				DestPixel->B = (uint8)(DestPixel->B*(1.0f-t) + SourcePixel->B*t);
			}
		}
	}
}

#endif //RENDERER_CPP
