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

struct rectangle{
	int32 X;
	int32 Y;
	int32 W;
	int32 H;
};

#endif //RENDERER_H
