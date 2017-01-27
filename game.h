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

struct transition_slid{
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
	transition_slid 		Slide;
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
	rectangle Source;
};

struct level{
	uint32 Occupancy[LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	tile Tiles[LEVEL_MAX_LAYER_COUNT][LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	uint32 Width;
	uint32 Height;
	snake  Snakes[SNAKE_MAX_COUNT];
	uint32 SnakeCount;
	vec2i  PortalPs[2];
	vec2i  GoalP;
	uint32 FruitCount;
	int32 ForegroundLayerIndex;
};

enum game_mode{
	Game_Mode_Play,
	Game_Mode_Edit,
	Game_Mode_Tile,
};

struct marked_snakes{
	snake 	*MarkedSnakes[SNAKE_MAX_COUNT];
	uint8 	IsMarked[SNAKE_MAX_COUNT];
	int32 	Count;
};

struct game_state{
	snake	*Player;

	level 	Level;
	uint32	LevelIndex;
	uint32 	LevelCount;
	char 	CurrentLevelName[2];

	real32 	t;

	marked_snakes MarkedSnakes;

	uint32 	ActiveBrush;

	tile	ActiveTileBrush;
	rectangle SpriteAtlasRect;
	bool32 SpriteAtlasActive;
	int32 ActiveLayerIndex;

	loaded_bitmap SpriteAtlas;

	game_mode Mode;
	uint32	MagicChecksum;
};

#endif //TYPES_H
