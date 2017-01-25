#if !defined(TYPES_H)
#define TYPES_H

enum basic_tyle_type{
	Tile_Type_Empty,
	Tile_Type_Solid,
	Tile_Type_Fruit,
	Tile_Type_Spikes,
	Tile_Type_Goal,
};

struct tile{
	rectangle Source;
};

struct snake_part{
	vec2i 	GridP;	//Cartesian coord (first quadrant)
};

struct snake{
	uint32 		Length;
	uint32		PaletteIndex;
	snake_part 	Parts[SNAKE_MAX_LENGTH];
	uint32		SnakeID;
};

struct level{
	uint32 Occupancy[LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	tile Tiles[LEVEL_MAX_LAYER_COUNT][LEVEL_MAX_WIDTH][LEVEL_MAX_HEIGHT];
	uint32 Width;
	uint32 Height;
	snake  Snakes[SNAKE_MAX_COUNT];
	uint32 SnakeCount;
	int32 ForegroundLayerIndex;
};

enum game_mode{
	Game_Mode_Play,
	Game_Mode_Edit,
	Game_Mode_Tile,
};

struct game_state{
	snake	*Player;

	level 	Level;
	uint32	LevelIndex;
	level 	Levels[LEVEL_COUNT];
	char 	CurrentLevelName[2];
	uint32 	LevelCount;

	snake 	*VisitedSnakes[SNAKE_MAX_COUNT];
	uint8 	Visited[SNAKE_MAX_COUNT];
	uint32 	VisitedCount;

	uint32	FruitRemaining;
	vec2i	PortalP;

	uint32 	ActiveBrush;
	vec2i 	MouseGridP;

	tile	ActiveTileBrush;
	rectangle SpriteAtlasRect;
	bool32 SpriteAtlasActive;
	int32 ActiveLayerIndex;

	loaded_bitmap SpriteAtlas;

	game_mode Mode;
	uint32	MagicChecksum;
};

#endif //TYPES_H
