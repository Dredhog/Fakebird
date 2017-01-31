#if !defined(TYPES_H)
#define TYPES_H

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

struct snake_part{
	vec2i 	GridP;	//Cartesian coord (first quadrant)
};

enum transition_type{
	Transition_Type_None,
	Transition_Type_Slide,
	Transition_Type_GotPushed,
	Transition_Type_Fall,
	Transition_Type_Teleportation,
	Transition_Type_FinishedLevel,
};

struct transition_slide{
	transition_type Type;
	vec2i NewHeadP;
};

struct transition_got_pushed{
	transition_type Type;
	vec2i Direction;
};

struct transition_fall{
	transition_type Type;
	vec2i FallRadius;
};

struct transition_teleported{
	transition_type Type;
	uint32 SourcePortalID;
};

union transition{
	transition_type 		Type;
	transition_slide 		Slide;
	transition_fall			Fall;
	transition_got_pushed 	GotPushed;
	transition_teleported	Teleportation;
};

struct snake{
	int32 		Length;
	uint32		SnakeID;
	snake_part 	Parts[SNAKE_MAX_LENGTH];
	transition	Transition;
	uint32		PaletteIndex;
	bool32 		IsOnPortal;
};

enum basic_tyle_type{
	Tile_Type_Empty,
	Tile_Type_Solid,
	Tile_Type_Fruit,
	Tile_Type_Spikes,
	Tile_Type_Goal,
	Tile_Type_PortalOne,
	Tile_Type_PortalTwo,
};

struct tile{
	loaded_bitmap *Bitmap;
	rectangle SourceRect;
};

struct mobile_sprite{
	tile  Sprite;
	vec3f P;
	vec3f dP;
};

struct level{
	uint32 	Width;
	uint32 	Height;
	uint32 	Occupancy[LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	tile 	Tiles[LEVEL_MAX_LAYER_COUNT][LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	snake  	Snakes[SNAKE_MAX_COUNT];
	uint32 	SnakeCount;
	vec2i  	GoalP;
	vec2i  	PortalPs[2];
	uint32 	FruitCount;
	int32 	ForegroundLayerIndex;
};

enum game_mode{
	Game_Mode_Play,
	Game_Mode_Edit,
	Game_Mode_Tile,
	Game_Mode_Overworld,
};

struct marked_snakes{
	snake 	*Snakes[SNAKE_MAX_COUNT];
	uint8 	IsMarked[SNAKE_MAX_COUNT];
	int32 	Count;
};

struct level_info{
	bool 	Exists;
};

struct overworld{
	char 		ActiveLevelName[3];
	uint32 		LevelCount;
	level_info	LevelInfos[LEVEL_MAX_COUNT];
};


struct projection{
	vec3f	CameraP;
	real32	UnitInPixels;
	real32	AspectRatio;
};

struct game_state{
	snake		*Player;

	overworld 	Overworld;
	level 		Level;
	uint32		LevelIndex;

	projection 	Projection;

	real32 	t;

	marked_snakes MarkedSnakes;

	uint32 		ActiveBrush;

	tile		ActiveTileBrush;
	rectangle 	SpriteAtlasDestRect;
	bool32 		SpriteAtlasActive;
	int32 		ActiveLayerIndex;

	loaded_bitmap Bitmaps[BITMAP_MAX_COUNT];
	int32	      CurrentBitmapIndex;
	int32		  BitmapCount;

	game_mode 	Mode;
	uint32		MagicChecksum;
};

#endif //TYPES_H
