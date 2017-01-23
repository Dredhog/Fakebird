#if !defined(RENDERER_H)
#define RENDERER_H

struct color{
	union{
		struct{
			uint8 A;
			uint8 B;
			uint8 G;
			uint8 R;
		};
		uint32 All;
	};
};

union mat2{
	struct{
		real32 Xx;
		real32 Xy;
		real32 Yx;
		real32 Yy;
	};
	struct{
		vec2f X;
		vec2f Y;
	};
	struct{
		real32 A;
		real32 B;
		real32 C;
		real32 D;
	};
};

struct parallelogram{
	vec2f P;
	union{
		mat2 Basis;
		struct{
			vec2f X;
			vec2f Y;
		};
	};
};

union rectangle{
	struct{
		vec2i MinP;
		vec2i MaxP;
	};
	struct{
		int32 MinX;
		int32 MinY;
		int32 MaxX;
		int32 MaxY;
	};
	int32 All[4];
};

#endif //RENDERER_H
