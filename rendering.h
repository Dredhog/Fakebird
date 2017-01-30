#if !defined(RENDERER_H)
#define RENDERER_H

struct rgba{
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 A;
};

struct abgr{
	uint8 A;
	uint8 B;
	uint8 G;
	uint8 R;
};

struct color{
	union{
		abgr ABGR;
		rgba RGBA;
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

#endif //RENDERER_H
