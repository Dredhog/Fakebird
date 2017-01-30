#if !defined(RENDERER_CPP)
#define RENDERER_CPP

internal void 
ClearOffscreenBuffer(offscreen_buffer Buffer, color FillColor){
	//BEGIN_TIMED_BLOCK(ClearOffscreenBuffer);
#if 1
	memset(Buffer.Memory, 200, Buffer.Width*Buffer.Height*sizeof(4));
#else
	FillColor = color{FillColor.R, FillColor.G, FillColor.B, FillColor.A};
	for(int32 Y = 0; Y < Buffer.Height; Y++){
		color *Pixels = (color*)((uint8*)Buffer.Memory + Y*Buffer.Pitch);
		for(int32 X = 0; X < Buffer.Width; X++){
			*Pixels = FillColor;
			Pixels++;
		}
	}
#endif
	//END_TIMED_BLOCK(ClearOffscreenBuffer);
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
	if(X >= Rect.MinX && X < Rect.MaxX && Y >= Rect.MinY && Y < Rect.MaxY){
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
	int32 MinX = MaxInt32(Rect.MinX, 0);
	int32 MinY = MaxInt32(Rect.MinY, 0);
	int32 MaxX = MinInt32(Rect.MaxX, (int32)Buffer.Width);
	int32 MaxY = MinInt32(Rect.MaxY, (int32)Buffer.Height);

	for(int32 Y = MinY; Y < MaxY; Y++){
		color *Pixels = (color*)((uint8*)Buffer.Memory + Y*Buffer.Pitch);
		for(int32 X = MinX; X < MaxX; X++){
			color *DestPixel = Pixels+X;
			real32 t = ((real32)FillColor.RGBA.A)/255.0f;
			DestPixel->ABGR.R = (uint8)(DestPixel->ABGR.R*(1.0f-t) + FillColor.RGBA.R*t);
			DestPixel->ABGR.G = (uint8)(DestPixel->ABGR.G*(1.0f-t) + FillColor.RGBA.G*t);
			DestPixel->ABGR.B = (uint8)(DestPixel->ABGR.B*(1.0f-t) + FillColor.RGBA.B*t);
		}
	}
}

internal void
DrawRectOutline(offscreen_buffer Buffer, rectangle Rect, color FillColor){
	FillColor = color{FillColor.ABGR.R, FillColor.ABGR.G, FillColor.ABGR.B, FillColor.ABGR.A};
	//top line
	if(Rect.MinY >= 0 && Rect.MinY < Buffer.Height){
		int32 MinX = MaxInt32(Rect.MinX, 0);
		int32 MaxX = MinInt32(Rect.MaxX, Buffer.Width);

		color *Color = (color*)Buffer.Memory + Rect.MinY*Buffer.Width;
		for(int32 X = MinX; X < MaxX; X++){
			*(Color + X) = FillColor;
		}
	}
	//bottom line
	if(Rect.MaxY-1 >= 0 && Rect.MaxY-1 < Buffer.Height){
		int32 MinX = MaxInt32(Rect.MinX, 0);
		int32 MaxX = MinInt32(Rect.MaxX, Buffer.Width);

		color *Color = (color*)Buffer.Memory + (Rect.MaxY-1)*Buffer.Width;
		for(int32 X = MinX; X < MaxX; X++){
			*(Color + X) = FillColor;
		}

	}
	//left line
	if(Rect.MinX >= 0 && Rect.MinX < Buffer.Width){
		int32 MinY = MaxInt32(Rect.MinY, 0);
		int32 MaxY = MinInt32(Rect.MaxY, Buffer.Height);

		color *Color = (color*)Buffer.Memory + Rect.MinX;
		for(int32 Y = MinY; Y < MaxY; Y++){
			*(Color + Y*Buffer.Width) = FillColor;
		}
	}
	//right line
	if(Rect.MaxX-1 >= 0 && Rect.MaxX-1 < Buffer.Width){
		int32 MinY = MaxInt32(Rect.MinY, 0);
		int32 MaxY = MinInt32(Rect.MaxY, Buffer.Height);

		color *Color = (color*)Buffer.Memory + (Rect.MaxX-1);
		for(int32 Y = MinY; Y < MaxY; Y++){
			*(Color + Y*Buffer.Width) = FillColor;
		}
	}
}

#if 0
internal void
DirectBitmapBliet(offscreen_buffer Buffer, vec2i Dest, loaded_bitmap Bitmap, rectangle Src){
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

internal inline bool
DoRectsIntersect(rectangle A, rectangle B){
	if(A.MaxX < B.MinX || B.MaxX < A.MinX ||
	   A.MaxY < B.MinY || B.MaxY < A.MinY){
		return false;
	}
	return true;
}

internal inline rectangle
GetRectIntersection(rectangle A, rectangle B){
	if(DoRectsIntersect(A, B)){
		int32 MinX = MaxInt32(A.MinX, B.MinX);
		int32 MinY = MaxInt32(A.MinY, B.MinY);
		int32 MaxX = MinInt32(A.MaxX, B.MaxX);
		int32 MaxY = MinInt32(A.MaxY, B.MaxY);
		return {MinX, MinY, MaxX, MaxY};
	}
	return {};
}

internal void
StretchBitmapOrthogonaly(offscreen_buffer Buffer, loaded_bitmap Bitmap, rectangle Dest, rectangle Source){
	//BEGIN_TIMED_BLOCK(StretchBitmapOrthogonaly);
	int32 SourceWidth = Source.MaxX - Source.MinX;
	int32 SourceHeight = Source.MaxY - Source.MinY;
	int32 DestWidth = Dest.MaxX - Dest.MinX;
	int32 DestHeight = Dest.MaxY - Dest.MinY;

	assert(SourceWidth >= 0 && SourceHeight >= 0);
	assert(DestWidth >= 0 && DestHeight >= 0);

	real32 	ScaleX = ((real32)SourceWidth)/((real32)DestWidth);
	real32 	ScaleY = ((real32)SourceHeight)/((real32)DestHeight);
	
	rectangle DestRelativeClippedRect = GetRectIntersection(rectangle{0, 0, Buffer.Width, Buffer.Height}, Dest);
	DestRelativeClippedRect.MinP -= Dest.MinP;
	DestRelativeClippedRect.MaxP -= Dest.MinP;

	rectangle SourceRelativeClippedRect = GetRectIntersection(rectangle{0, 0, Bitmap.Width, Bitmap.Height}, Source);
	SourceRelativeClippedRect.MinP -= Source.MinP;
	SourceRelativeClippedRect.MaxP -= Source.MinP;

	color *DestStart = (color*)Buffer.Memory + Dest.MinY*Buffer.Width + Dest.MinX;
	color *SourceStart = (color*)Bitmap.Texels + Source.MinY*Bitmap.Width + Source.MinX;

	for(int32 Y = DestRelativeClippedRect.MinY; Y < DestRelativeClippedRect.MaxY; Y++){
		for(int32 X = DestRelativeClippedRect.MinX; X < DestRelativeClippedRect.MaxX; X++){
			vec2i SourceP = {(int32)(ScaleX*(real32)X), (int32)(ScaleY*(real32)Y)};

			if(IsPointInsideRect(SourceP, SourceRelativeClippedRect)){
				color *DestPixel = DestStart + Y*Buffer.Width + X;
				color *SourcePixel = SourceStart + SourceP.Y*Bitmap.Width + SourceP.X;

				real32 t = ((real32)SourcePixel->ABGR.A)/255.0f;
				DestPixel->ABGR.R = (uint8)(DestPixel->ABGR.R*(1.0f-t) + SourcePixel->ABGR.R*t);
				DestPixel->ABGR.G = (uint8)(DestPixel->ABGR.G*(1.0f-t) + SourcePixel->ABGR.G*t);
				DestPixel->ABGR.B = (uint8)(DestPixel->ABGR.B*(1.0f-t) + SourcePixel->ABGR.B*t);
			}
		}
	}
	//END_TIMED_BLOCK(StretchBitmapOrthogonaly);
}

#if 0
internal rectangle
GetParallelogramBoundingRect(parallelogram Para){
	vec2f MinP = Para.P;
	vec2f MaxP = Para.P;
	vec2f Points[3] = {	Para.P + Para.X,
	   					Para.P + Para.Y,
					   	Para.P + Para.X + Para.Y};
	for(int32 i = 0; i < 3; i++){
		if(Points[i].X < MinP.X){
			MinP.X = Points[i].X;
		}
		if(Points[i].Y < MinP.Y){
			MinP.Y = Points[i].Y;
		}
		if(Points[i].X > MaxP.X){
			MaxP.X = Points[i].X;
		}
		if(Points[i].Y > MaxP.Y){
			MaxP.Y = Points[i].Y;
		}
	}

	rectangle Result = {(int32)MinP.X, (int32)MinP.Y, (int32)MaxP.X, (int32)MaxP.Y};
	return Result;
}


internal inline vec2f
Mul(mat2 Mat, vec2f V){
	vec2f Result = {Mat.Xx*V.X + Mat.Yx*V.Y,
					Mat.Xy*V.X + Mat.Yy*V.Y};
	return Result;
}

internal inline mat2 
Inv(mat2 Mat){
	real32 Det = 1.f/(Mat.A*Mat.D-Mat.B*Mat.C);
	mat2 Result;
	Result.A = Det*Mat.D;
	Result.B = Det*(-Mat.B);
	Result.C = Det*(-Mat.C);
	Result.D = Det*Mat.A;
	return Result;
}

internal inline vec2f
GetNormalisedUVCoordinates(int32 X, int32 Y, parallelogram Para){
	mat2 InvBasis = Inv(Para.Basis);
	vec2f RelP = vec2f{(real32)X, (real32)Y} - Para.P;
	vec2f NormP = Mul(InvBasis, RelP);
	return NormP;
}

internal void
BlitBitmap(offscreen_buffer Buffer, loaded_bitmap Bitmap, parallelogram Dest, parallelogram Source){
	rectangle DestBoundingRect = GetParallelogramBoundingRect(Dest);
	rectangle ClippedDestBoundingRect = GetRectIntersection(rectangle{0, 0, Buffer.Width, Buffer.Height}, DestBoundingRect);

	rectangle BitmapRect = {0, 0, Bitmap.Width, Bitmap.Height};
	color *DestStart = (color*)Buffer.Memory;
	color *SourceStart = (color*)Bitmap.Texels;
	for(int32 Y = ClippedDestBoundingRect.MinY; Y < ClippedDestBoundingRect.MaxY; Y++){
		for(int32 X = ClippedDestBoundingRect.MinX; X < ClippedDestBoundingRect.MaxX; X++){
			vec2f NormUV = GetNormalisedUVCoordinates(X, Y, Dest);
			if(NormUV.X > 0.0f && NormUV.X < 1.0f && NormUV.Y > 0.0f && NormUV.Y < 1.0f){
				vec2f UVf = Mul(Source.Basis, NormUV);
				vec2i UV = {(int32)UVf.X, (int32)UVf.Y};
				if(IsPointInsideRect(UV, BitmapRect)){
					color *SourceTexel = SourceStart + UV.Y*Bitmap.Width + UV.X;
					color *DestPixel = DestStart + Y*Buffer.Width + X;

					real32 t = ((real32)SourceTexel->A)/255.0f;
					DestPixel->ABGR.R = (uint8)(DestPixel->ABGR.R*(1.0f-t) + SourceTexel->ABGR.R*t);
					DestPixel->ABGR.G = (uint8)(DestPixel->ABGR.G*(1.0f-t) + SourceTexel->ABGR.G*t);
					DestPixel->ABGR.B = (uint8)(DestPixel->ABGR.B*(1.0f-t) + SourceTexel->ABGR.B*t);
				}
			}
		}
	}
}
#endif

#endif //RENDERER_CPP
