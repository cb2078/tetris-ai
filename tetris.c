#include <stdbool.h>

#define HEIGHT 22
#define WIDTH 10

#include "shape.c"
#include "levels.c"

int shape_queue[2];
#define current_shape	(shape_queue[0])
#define next_shape	(shape_queue[1])

int x, y, r;
#define SHAPE shapes[current_shape][r]

int lines = 0;
int level = 15;
int score = 0;
int soft_score = 0;

bool spawn = true;

typedef Board[HEIGHT][WIDTH];
Board board;

static bool collides(int x, int y, int r)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			if (!SHAPE[i][j])
				continue;
			int iy = i + y;
			int jx = j + x;
			if (iy < 0 || iy >= HEIGHT || jx < 0 || jx >= WIDTH || board[iy][jx])
				return true;
		}
	return false;
}

static unsigned short random_int(void)
{
	static unsigned short seed = 2;
	int x = 1 & seed >> 9;
	int y = 1 & seed >> 1;
	int left = x ^ y;
	seed = left << 15 | seed >> 1;
	return seed;
}

static int random_shape(void)
{
	static int shapes = 0;
	static int spawn_id[N_SHAPES] = { 0x12, 0x0a, 0x0b, 0x08, 0x07, 0x0e, 0x02, };

	unsigned char i = random_int() >> 8;
	i += ++shapes;
	i &= 7;
	if (7 == i || i == current_shape)
	{
		i = random_int() >> 8;
		i &= 7;
		i += current_shape;
		i %= 7;
	}
	return i;
}

static bool spawn_shape(void)
{
	shape_queue[0] = shape_queue[1];
	shape_queue[1] = random_shape();
	x = 3;
	y = 1;
	r = 0;
	return !collides(x, y, r);
}

static void inc_level(void)
{
	if (lines >= level * 10 + 10 || lines >= 100 && lines >= level * 10 - 50)
		level += lines % 10 == 0;
}

static bool row(int i)
{
	for (int j = 0; j < WIDTH; ++j)
		if (!board[i][j])
			return false;
	++lines;
	inc_level();
	return true;
}

static int points_per_line[] = { 0, 40, 100, 300, 1200, };
static void clear(void)
{
	bool clears[HEIGHT];
	int l = 0;
	for (int i = 0; i < HEIGHT; ++i)
	{
		l += clears[i] = row(i);
	}

	score += points_per_line[l] * (level + 1) + soft_score;
	soft_score = 0;

	for (int i = HEIGHT - 1, k = 0; i >= 0; --i)
	{
		while (clears[i - k])
			++k;
		for (int j = 0; j < WIDTH; ++j)
			board[i][j] = i - k < 0 ? 0 : board[i - k][j];
	}
}

static void write(void)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (SHAPE[i][j])
				board[y + i][x + j] = SHAPE[i][j];
	clear();
}

static bool move(int dx, int dy, int dr)
{
	if (collides(x + dx, y + dy, (r + dr) % 4))
	{
		if (dy)
		{
			write();
			spawn = true;
		}
		return false;
	}
	x += dx;
	y += dy;
	r = (r + dr + 4) % 4;
	return true;
}

static void init()
{
	next_shape = random_shape();
}
