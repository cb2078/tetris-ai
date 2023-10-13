#include "raylib.h"

#define HEIGHT 22
#define WIDTH 10

#include "shape.c"
#include "levels.c"

int shape, next_shape;
int x, y, r;
#define SHAPE shapes[shape][r]
		
int lines = 69;
int level = 6;
int score = 0;

bool spawn = true;

int board[HEIGHT][WIDTH];

static void draw_cell(int x, int y, int c)
{
	Color colours[N_SHAPES + 1] = {
		DARKGRAY,
		RED,
		ORANGE,
		YELLOW,
		GREEN,
		BLUE,
		PURPLE,
		VIOLET,
	};
	static int width = 20;
	static int boarder = 1;
	static int offset = 1;
	DrawRectangle((x + offset) * (width + boarder), (y + offset) * (width + boarder), width, width, colours[c]);
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

static int random_shape(void)
{
	return GetRandomValue(0, N_SHAPES - 1);
}

static bool spawn_shape(void)
{
	shape = next_shape;
	next_shape = random_shape();
	x = 4;
	y = 0;
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

static int points_per_line[] = { 40, 100, 300, 1200, };
static void clear(void)
{
	bool clears[HEIGHT];
	int l = 0;
	for (int i = 0; i < HEIGHT; ++i)
	{
		l += clears[i] = row(i);
	}

	score += points_per_line[l] * (level + 1);

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

int main(void)
{
	InitWindow(800, 600, "tetris ai");
	SetTargetFPS(60);

	next_shape = random_shape();
	bool running = true;
	int last_drop = 0;
	int frames = 0;
	int das = 0;
	int last_dir = 0;
	int soft = 0;

	while (!WindowShouldClose())
	{
		if (!running)
			goto render;
		if (spawn)
		{
			running &= spawn_shape();
			spawn = false;
		}
		if (frames - last_drop == drop_speed(level))
		{
			move(0, 1, 0);
			last_drop = frames;
		}

		int dir = IsKeyDown(KEY_LEFT) ? -1 : IsKeyDown(KEY_RIGHT) ? 1 : 0;
		das = dir == 0 ? 0 : dir == last_dir ? das + 1 : 1;
		last_dir = dir;
		if (das == 1 || das > 16 && das % 6 == 5)
			move(dir, 0, 0);
		
		if (IsKeyDown(KEY_DOWN))
		{
			++soft;
			if (soft % 3 == 0)
				move(0, 1, 0);
		}
		else
			soft = 0;
		if (IsKeyPressed(KEY_UP))
			while (move(0, 1, 0));
		if (IsKeyPressed(KEY_Q))
			move(0, 0, 1);
		if (IsKeyPressed(KEY_E))
			move(0, 0, -1);

render:
		BeginDrawing();
		ClearBackground(BLACK);
		
		// board
		for (int i = 0; i < HEIGHT; ++i)
			for (int j = 0; j < WIDTH; ++j)
				draw_cell(j, i, board[i][j]);
		// next box
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				draw_cell(j + WIDTH + 1, i, shapes[next_shape][0][i][j]);
		// current shape
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (SHAPE[i][j])
					draw_cell(x + j, y + i, SHAPE[i][j]);
		
		// debug
		DrawText(TextFormat("running:\t%s\npos:\t%d %d %d\ndas:\t%d\ndir:\t%d\nlevel:\t%d\nlines:\t%d\nscore:\t%d",
		                    running ? "true" : "false",
		                    x, y, r,
		                    das, dir,
		                    level, lines, score),
		         400, 10, 20, ORANGE);

		for (int k = 0; k < N_SHAPES; ++k)
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					draw_cell(WIDTH + 1 + k % 4 * 5 + j, 13 + i + k / 4 * 5, shapes[k][0][i][j]);
		EndDrawing();
		
		++frames;
	}
	return 0;
}
