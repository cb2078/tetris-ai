#define HEIGHT 22
#define WIDTH 10

#include "shape.c"
#include "levels.c"

int shape_queue[2];
#define current_shape	(shape_queue[0])
#define next_shape	(shape_queue[1])

int x, y, r;

int frames = 0;
int lines = 0;
int level = 19;

typedef int board_t[HEIGHT][WIDTH];
board_t board;

static void print_board(board_t board)
{
	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int j = 0; j < WIDTH; ++j)
			putchar(board[i][j] ? board[i][j] + '0' : '.');
		putchar('\n');
	}
}

static int board_height(board_t board)
{
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			if (board[i][j])
				return HEIGHT - i;
	return 0;
}

static float board_variance(board_t board)
{
	int sum = 0;
	int heights[WIDTH] = {0};
	for (int j = 0; j < WIDTH; ++j)
	{
		for (int i = 0; i < HEIGHT; ++i)
			if (board[i][j])
			{
				sum += heights[j] = HEIGHT - i;
				break;
			}
	}
	float avg = (float)sum / 10;
	float variance = 0;
	for (int j = 0; j < WIDTH; ++j)
	{
		float diff = heights[j] - avg;
		variance += diff > 0 ? diff : -diff;
	}
	return variance;
}

static int board_holes(board_t board)
{
	int holes = 0;
	for (int j = 0; j < WIDTH; ++j)
		for (int i = 0, flag = 0; i < HEIGHT; ++i)
			if (board[i][j])
				flag = 1;
			else
				holes += flag;
	return holes;
}

static bool board_is_empty(board_t board)
{
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			if (board[i][j])
				return false;
	return true;
}

static bool collides(board_t board, int shape, int x, int y, int r)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			if (!shapes[shape][r][i][j])
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
	unsigned short x = 1 & seed >> 9;
	unsigned short y = 1 & seed >> 1;
	unsigned short left = x ^ y;
	seed = left << 15 | seed >> 1;
	return seed;
}

static int random_shape(void)
{
	static int shapes = 0;
	static int spawn_id[N_SHAPES] = { 0x12, 0x0a, 0x0b, 0x08, 0x07, 0x0e, 0x02, };

	unsigned char i = random_int() >> 8;
	i += (unsigned char)++shapes;
	i &= 7;
	if (7 == i || i == current_shape)
	{
		i = random_int() >> 8;
		i &= 7;
		i += (unsigned char)current_shape;
		i %= 7;
	}
	return i;
}

static bool spawn_shape(void)
{
	current_shape = next_shape;
	next_shape = random_shape();
	x = 3;
	y = 1;
	r = 0;
	frames = 0;
	return !collides(board, current_shape, x, y, r);
}

static void inc_level(void)
{
	if (lines >= level * 10 + 10 || lines >= 100 && lines >= level * 10 - 50)
		level += lines % 10 == 0;
}

static bool row(board_t board, int i)
{
	for (int j = 0; j < WIDTH; ++j)
		if (!board[i][j])
			return false;
	return true;
}

static int points_per_line[] = { 0, 40, 100, 300, 1200, };
static int clear(board_t board)
{
	bool clears[HEIGHT];
	int l = 0;
	for (int i = 0; i < HEIGHT; ++i)
		l += clears[i] = row(board, i);

	for (int i = HEIGHT - 1, k = 0; i >= 0; --i)
	{
		while (clears[i - k])
			++k;
		for (int j = 0; j < WIDTH; ++j)
			board[i][j] = i - k < 0 ? 0 : board[i - k][j];
	}

	return l;
}

static int write(board_t board, int shape, int x, int y, int r)
{
	assert(!collides(board, shape, x, y, r));
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			int cell = shapes[shape][r][i][j];
			if (cell)
				board[y + i][x + j] = cell;
		}
	return clear(board);
}

static bool move(int dx, int dy, int dr)
{
	if (collides(board, current_shape, x + dx, y + dy, (r + dr + 4) % 4))
		return false;
	x += dx;
	y += dy;
	r = (r + dr + 4) % 4;
	return true;
}

static void init()
{
	next_shape = random_shape();
	spawn_shape();
}
