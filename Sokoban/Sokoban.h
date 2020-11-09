#define MAP_WIDTH 30
#define MAP_HEIGHT 30
//∏  ªÁ¿Ã¡Ó (MAP_WIDTH X MAP_HEIGHT)

#define Key_h 104
#define Key_j 106
#define Key_k 107
#define Key_l 108
#define Key_u 117
#define Key_r 114
#define Key_n 110
#define Key_e 101
#define Key_s 115
#define Key_f 102
#define Key_d 100
#define Key_t 116

struct Position
{
	int pos_X;
	int pos_Y;
};

enum Movement
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

enum BlockType
{
	BLANK,
	WALL,
	GOLD,
	GOAL
};
