#if !defined(TRI_RENDERER_CPP)
#define TRI_RENDERER_CPP

internal real32
Orient2d(vec2f A, vec2f B, vec2f C){
	return (B.X - A.X)*(C.Y - A.Y) - (B.Y - A.Y)*(C.X - A.X);
	//return (A.Y - B.Y)*C.X + (B.X - A.X)*C.Y + (A.X*B.Y - A.Y*B.X);
}

#if 1
internal void
DrawTriangle(offscreen_buffer Target, loaded_bitmap Source, vec3 V0, vec3 V1, vec3 V2, vec2f UV0, vec2f UV1, vec2f UV2){
	BEGIN_TIMED_BLOCK(DrawTriangle);
	//SwapOrder first two verts and UVs for now
	vec3 TempV = V0;
	V0 = V1;
	V1 = TempV;
	vec2f TempUV = UV0;
	UV0 = UV1;
	UV1 = TempUV;
	

	real32 SignedArea = Orient2d(V0.XY, V1.XY, V2.XY);
	if(SignedArea <= 0){
		return;
	}
	rectangle Bounds = {};
	Bounds.MinP = {Min3Real32(V0.X, V1.X, V2.X), Min3Real32(V0.Y, V1.Y, V2.Y)};
	Bounds.MaxP = {Max3Real32(V0.X, V1.X, V2.X), Max3Real32(V0.Y, V1.Y, V2.Y)};
	rectangle ClippedBounds = GetRectIntersection(Bounds, {0.0f, 0.0f, (real32)Target.Width, (real32)Target.Height});


	real32 A01 = V0.Y - V1.Y; real32 B01 = V1.X - V0.X;
	real32 A12 = V1.Y - V2.Y; real32 B12 = V2.X - V1.X;
	real32 A20 = V2.Y - V0.Y; real32 B20 = V0.X - V2.X;

	real32 InvArea = 1.0f/SignedArea;
	A01 *= InvArea;
	A12 *= InvArea;
	A20 *= InvArea;
	B01 *= InvArea;
	B12 *= InvArea;
	B20 *= InvArea;

	real32 OneOverV0Z = 1.0f/(V0.Z);
	real32 OneOverV1Z = 1.0f/(V1.Z);
	real32 OneOverV2Z = 1.0f/(V2.Z);

	vec2f  UV0Prime = UV0*(-OneOverV0Z);
	vec2f  UV1Prime = UV1*(-OneOverV1Z);
	vec2f  UV2Prime = UV2*(-OneOverV2Z);

	vec2f StartP = {ceilf(ClippedBounds.MinX), ceilf(ClippedBounds.MinY)};
	vec2f EndP = {ceilf(ClippedBounds.MaxX), ceilf(ClippedBounds.MaxY)};

	vec2f PixelOffset = Bounds.MinP - vec2f{ceilf(Bounds.MinX), ceilf(Bounds.MinY)};
	real32 W0_Row = Orient2d(V1.XY, V2.XY, StartP)*InvArea;
	real32 W1_Row = Orient2d(V2.XY, V0.XY, StartP)*InvArea;
	real32 W2_Row = Orient2d(V0.XY, V1.XY, StartP)*InvArea;

	for(vec2f P = StartP; P.Y < EndP.Y; P.Y++){

		real32 W0 = W0_Row;
		real32 W1 = W1_Row;
		real32 W2 = W2_Row;

		for(P.X = StartP.X; P.X < EndP.X; P.X++){
			if(W0 >= 0 && W1 >= 0 && W2 >= 0){

#define SAMPLE_TEXTURE 1
#if SAMPLE_TEXTURE 

#define PERSPECTIVE_TEXTURE_MAPPING 1
#if PERSPECTIVE_TEXTURE_MAPPING 
				real32 ZPrime = W0*OneOverV0Z + W1*OneOverV1Z + W2*OneOverV2Z;
				vec2f SamplePoint = (UV0Prime*W0 + UV1Prime*W1 + UV2Prime*W2)/ZPrime;
#else
				vec2f SamplePoint = UV0*W0 + UV1*W1 + UV2*W2;
#endif

#define TEXTURE_CLAMP_EDGE 0
#if TEXTURE_CLAMP_EDGE 
				SamplePoint.X = ClampReal32(SamplePoint.X, 0.0f, 1.0f);
				SamplePoint.Y = ClampReal32(SamplePoint.Y, 0.0f, 1.0f);
#else
				SamplePoint.X = SamplePoint.X - floorf(SamplePoint.X);
				SamplePoint.Y = SamplePoint.Y - floorf(SamplePoint.Y);
#endif

				assert(SamplePoint.X >= 0.0f && SamplePoint.X <= 1.0f);
				assert(SamplePoint.Y >= 0.0f && SamplePoint.Y <= 1.0f);
				SamplePoint.X *= (real32)Source.Width;
				SamplePoint.Y *= (real32)Source.Height;

				color *SourcePixel = (color*)Source.Texels + (int32)SamplePoint.Y*Source.Width + (int32)SamplePoint.X;
				color *DestPixel = (color*)Target.Memory + (int32)P.Y*Target.Width + (int32)P.X;
				*DestPixel = *SourcePixel;
#else
				color *DestPixel = (color*)Target.Memory + (int32)P.Y*Target.Width + (int32)P.X;
				*DestPixel = {255, (uint8)(255.f*W0), (uint8)(255.f*W1), (uint8)(255.f*W2)};
#endif
			}

			W0 += A12;
			W1 += A20;
			W2 += A01;
		}

		W0_Row += B12;
		W1_Row += B20;
		W2_Row += B01;
	}
	END_TIMED_BLOCK(DrawTriangle);
}

#else 
internal void
DrawTriangle(offscreen_buffer Target, loaded_bitmap Source, vec3 A, vec3 B, vec3 C, vec2f UVa, vec2f UVb, vec2f UVc){
	BEGIN_TIMED_BLOCK(DrawTriangle);
	if(Orient2d(A.XY, B.XY, C.XY) > 0){//Checking > 0 since projection makes coordinates left handed
		return;
	}
	rectangle Bounds = {};
	Bounds.MinP = {Min3Real32(A.X, B.X, C.X), Min3Real32(A.Y, B.Y, C.Y)};
	Bounds.MaxP = {Max3Real32(A.X, B.X, C.X), Max3Real32(A.Y, B.Y, C.Y)};
	rectangle ClippedBounds = GetRectIntersection(Bounds, {0, 0, (real32)Target.Width, (real32)Target.Height});

	vec2f StartP = {ceilf(ClippedBounds.MinX), ceilf(ClippedBounds.MinY)};
	vec2f EndP = {ceilf(ClippedBounds.MaxX), ceilf(ClippedBounds.MaxY)};

	for(vec2f P = StartP; P.Y < EndP.Y; ++P.Y){
		for(P.X = StartP.X; P.X < EndP.X; ++P.X){
			real32 W0 = Orient2d(B.XY, C.XY, P);
			real32 W1 = Orient2d(C.XY, A.XY, P);
			real32 W2 = Orient2d(A.XY, B.XY, P);

			if(W0 <= 0 && W1 <= 0 && W2 <= 0){ //Checking <= 0 since projection makes coordinates left handed
				real32 OneOverArea = 1.0f/(W0+W1+W2);
#define PERSPECTIVE_CORRECT_TEXTURE_MAPPING 1
#if PERSPECTIVE_CORRECT_TEXTURE_MAPPING 
				W0 *= OneOverArea;
				W1 *= OneOverArea;
				W2 *= OneOverArea;

				vec2f  UVaPrime = UVa/(-A.Z);
				vec2f  UVbPrime = UVb/(-B.Z);
				vec2f  UVcPrime = UVc/(-C.Z);
				real32 ZPrime = W0*(1.0f/A.Z) + W1*(1.0f/B.Z) + W2*(1.0f/C.Z);

				vec2f SamplePoint = (UVaPrime*W0 + UVbPrime*W1 + UVcPrime*W2)/ZPrime;

#else
				vec2f SamplePoint = (UVa*W0 + UVb*W1 + UVc*W2)*OneOverArea;
#endif
#define TEXTURE_CLAMP_EDGE 0
#if TEXTURE_CLAMP_EDGE 
				SamplePoint.X = ClampReal32(SamplePoint.X, 0.0f, 1.0f);
				SamplePoint.Y = ClampReal32(SamplePoint.Y, 0.0f, 1.0f);
#else
				SamplePoint.X = SamplePoint.X - floorf(SamplePoint.X);
				SamplePoint.Y = SamplePoint.Y - floorf(SamplePoint.Y);
#endif
				assert(SamplePoint.X >= 0.0f && SamplePoint.X <= 1.0f);
				assert(SamplePoint.Y >= 0.0f && SamplePoint.Y <= 1.0f);
				SamplePoint.X *= (real32)Source.Width;
				SamplePoint.Y *= (real32)Source.Height;

				color *SourcePixel = (color*)Source.Texels + (int32)SamplePoint.Y*Source.Width + (int32)SamplePoint.X;
				color *DestPixel = (color*)Target.Memory + (int32)P.Y*Target.Width + (int32)P.X;
				*DestPixel = *SourcePixel;//{255, (uint8)255.f*W0, (uint8)255.f*W1, (uint8)255.f*W2};
			}
		}
	}
	END_TIMED_BLOCK(DrawTriangle);
}
#endif

internal void
ColorTriangle(offscreen_buffer Target, vec2f A, vec2f B, vec2f C){
	BEGIN_TIMED_BLOCK(DrawTriangle);
	if(Orient2d(A, B, C) < 0){
		return;
	}

	rectangle Bounds = {};
	Bounds.MinP = {Min3Real32(A.X, B.X, C.X), Min3Real32(A.Y, B.Y, C.Y)};
	Bounds.MaxP = {Max3Real32(A.X, B.X, C.X), Max3Real32(A.Y, B.Y, C.Y)};
	rectangle ClippedBounds = GetRectIntersection(Bounds, {0, 0, (real32)Target.Width, (real32)Target.Height});

	vec2f StartP = {ceilf(ClippedBounds.MinX), ceilf(ClippedBounds.MinY)};
	vec2f EndP = {ceilf(ClippedBounds.MaxX), ceilf(ClippedBounds.MaxY)};

	for(vec2f P = StartP; P.Y < EndP.Y; ++P.Y){
		for(P.X = StartP.X; P.X < EndP.X; ++P.X){
			real32 W0 = Orient2d(B, C, P);
			real32 W1 = Orient2d(C, A, P);
			real32 W2 = Orient2d(A, B, P);

			if(W0 >= 0 && W1 >= 0 && W2 >= 0){
				real32 OneOverArea = 1.0f / (W0 + W1 + W2);
				W0 *= OneOverArea;
				W1 *= OneOverArea;
				W2 *= OneOverArea;

				color *DestPixel = (color*)Target.Memory + (int32)P.Y*Target.Width + (int32)P.X;
				*DestPixel = {255, (uint8)(255.f*W0), (uint8)(255.f*W1), (uint8)(255.f*W2)};
			}
		}
	}
	END_TIMED_BLOCK(DrawTriangle);
}
internal void
DrawTriangleMesh(offscreen_buffer TargetBuffer, loaded_bitmap Bitmap, triangle_mesh *Mesh, mat4 Transformation){

	for(uint32 i = 0; i < Mesh->IndiceCount; i += 3){
		assert(Mesh->Indices[i+0] < Mesh->VerticeCount);
		assert(Mesh->Indices[i+1] < Mesh->VerticeCount);
		assert(Mesh->Indices[i+2] < Mesh->VerticeCount);
		vec3 A 	  = Mesh->Vertices[Mesh->Indices[i+0]].P;
		vec3 B    = Mesh->Vertices[Mesh->Indices[i+1]].P;
		vec3 C 	  = Mesh->Vertices[Mesh->Indices[i+2]].P;
		vec4 NewA = MulMat4Vec4(Transformation, Vec4(A, 1.0f));
		vec4 NewB = MulMat4Vec4(Transformation, Vec4(B, 1.0f));
		vec4 NewC = MulMat4Vec4(Transformation, Vec4(C, 1.0f));


		NewA.XY *= -1/NewA.Z;
		NewB.XY *= -1/NewB.Z;
		NewC.XY *= -1/NewC.Z;

		NewA.X = (0.5f*NewA.X+0.5f)*(real32)TargetBuffer.Width;
		NewB.X = (0.5f*NewB.X+0.5f)*(real32)TargetBuffer.Width;
		NewC.X = (0.5f*NewC.X+0.5f)*(real32)TargetBuffer.Width;
		NewA.Y = (-0.5f*NewA.Y+0.5f)*(real32)TargetBuffer.Height;
		NewB.Y = (-0.5f*NewB.Y+0.5f)*(real32)TargetBuffer.Height;
		NewC.Y = (-0.5f*NewC.Y+0.5f)*(real32)TargetBuffer.Height;

		vec2f UVa = Mesh->Vertices[Mesh->Indices[i+0]].UV;
		vec2f UVb = Mesh->Vertices[Mesh->Indices[i+1]].UV;
		vec2f UVc = Mesh->Vertices[Mesh->Indices[i+2]].UV;
#if 1
		DrawTriangle(TargetBuffer, Bitmap, NewA.XYZ, NewB.XYZ, NewC.XYZ, UVa, UVb, UVc);
#else
		ColorTriangle(TargetBuffer, NewA.XY, NewB.XY, NewC.XY);
#endif
	}
}

#endif //TRI_RENDERER_CPP
