#include "raylib.h"

#define HEIGHT 22
#define WIDTH 10

#include "shape.c"
#include "levels.c"

int shape, next_shape;
int x, y, r;
#define SHAPE shapes[shape][r]

int lines = 0;
int level = 15;
int score = 0;
int soft_score = 0;

bool spawn = true;
int are = 0;
int clear_delay = 0;

static int das = 0;

typedef Board[HEIGHT][WIDTH];
Board board;

static void draw_cell(int x, int y, int c)
{
	Color colours[] = {
		DARKGRAY,
		RED,
		ORANGE,
		YELLOW,
		GREEN,
		BLUE,
		SKYBLUE,
		PINK,
		GRAY,
	};
	static int width = 20;
	static int boarder = 1;
	DrawRectangle((x + 1) * (width + boarder), (y - 1) * (width + boarder), width, width, colours[c]);
}

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
	if (7 == i || i == shape)
	{
		i = random_int() >> 8;
		i &= 7;
		i += shape;
		i %= 7;
	}
	return i;
}

static bool spawn_shape(void)
{
	shape = next_shape;
	next_shape = random_shape();
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

	// ARE
	int b = 21 - (y + bottom(shape, r));
	are = (b + 2) / 4 * 2 + 10;
	clear_delay = l ? 20 : 0;
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
		if (dx)
			das = 16;
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

typedef enum Input Input;
enum Input {
	UP	= 0x01,
	DOWN	= 0x02,
	LEFT	= 0x04,
	RIGHT	= 0x08,
	ROT_ACW	= 0x10,
	ROT_CW	= 0x20,
};
#define NUM_INPUTS 6

static void init()
{
	next_shape = random_shape();
}

// advance the game by one frame
static void advance(int input)
{
	static bool running = true;
	static int frames = 0;
	static int old_input = 0;
	static int soft = 0;

	if (!running)
		return;
	if (are)
	{
		--are;
		return;
	}
	if (clear_delay)
	{
		--clear_delay;
		return;
	}

	random_int();

	if (spawn)
	{
		running &= spawn_shape();
		spawn = false;
		frames = 0;
	}

	// rotate clockwise / anti clockwise
	switch (input & ~old_input & (ROT_CW | ROT_ACW)) {
		case ROT_CW:
			move(0, 0, -1);
			break;
		case ROT_ACW:
			move(0, 0, 1);
			break;
	}

	// hard drop
	if (input & ~old_input & UP)
		while (move(0, 1, 0));

	// soft drop
	if (input & DOWN)
	{
		++soft;
		if (3 == soft)
		{
			soft = 1;
			++soft_score;
			move(0, 1, 0);
		}
		return;
	}
	else  if (old_input & DOWN)
	{
		frames = soft;
		soft = 0;
		soft_score = 0;
	}

	// gravity
	if (drop_speed(level) <= frames)
	{
		frames = 0;
		move(0, 1, 0);
	}
	else
		++frames;

	int dir = input & RIGHT ? 1 : input & LEFT ? -1 : 0;
	// move left / right
	if (dir)
		if (input & ~old_input & (LEFT | RIGHT))
		{
			das = 0;
			move(dir, 0, 0);
		}
		else if (16 <= ++das)
		{
			das = 10;
			move(dir, 0, 0);
		}

	old_input = input;
}

static void run_window(void)
{
	InitWindow(800, 600, "tetris ai");
	SetTargetFPS(60);

	// keybinds are here
	static int keys[NUM_INPUTS] = {
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_Q,
		KEY_E,
	};

	init();

	while (!WindowShouldClose())
	{
		// keyboard events
		int input = 0;
		for (int i = 0; i < NUM_INPUTS; ++i)
			if (IsKeyDown(keys[i]))
				input |= 1 << i;

		advance(input);

		BeginDrawing();
		ClearBackground(BLACK);

		// board
		for (int i = 2; i < HEIGHT; ++i)
			for (int j = 0; j < WIDTH; ++j)
				draw_cell(j, i, board[i][j]);
		// next box
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				draw_cell(j + WIDTH + 1, i + 2, shapes[next_shape][0][i][j]);
		// current shape
		{
			int k = y;
			while (k < HEIGHT - 1 && !collides(x, 1 + k, r))
				++k;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (SHAPE[i][j] && !clear_delay)
					{
						draw_cell(x + j, k + i, N_SHAPES + 1); // shadow
						draw_cell(x + j, y + i, SHAPE[i][j]);
					}
		}
		// debug
		DrawText(TextFormat("pos:\t%d %d %d\nlevel:\t%d\nlines:\t%d\nscore:\t%d",
		                    x, y, r,
		                    level, lines, score),
		         400, 10, 20, ORANGE);
		for (int k = 0; k < N_SHAPES; ++k)
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					draw_cell(WIDTH + 1 + k % 4 * 5 + j, 13 + i + k / 4 * 5, shapes[k][0][i][j]);
		// das counter
		for (int i = 0; i <= 16; ++i)
			draw_cell(i, 23, i > das ? 0 : das < 10 ? 1 : 4);
		DrawText(TextFormat("das: %d", das), 21, 525, 20, ORANGE);
		EndDrawing();
	}
}

#include "search.c"

int main(void)
{
	init();
	spawn_shape();
	bfs();
	return 0;
}
