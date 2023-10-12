#include "raylib.h"

#define HEIGHT 22
#define WIDTH 10

#include "shape.c"

int shape, next_shape;
int x, y, r;
#define SHAPE shapes[shape][r]

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

static bool move(int dx, int dy, int dr)
{
	if (collides(x + dx, y + dy, (r + dr) % 4))
		return false;
	x += dx;
	y += dy;
	r = (r + dr + 4) % 4;
	return true;
}

static int random_shape()
{
	return GetRandomValue(0, N_SHAPES - 1);
}

static bool spawn_shape()
{
	shape = next_shape;
	next_shape = random_shape();
	x = 4;
	y = 0;
	r = 0;
	return !collides(x, y, r);
}

static bool row(int i)
{
	for (int j = 0; j < WIDTH; ++j)
		if (!board[i][j])
			return false;
	return true;
}

static void clear()
{
	bool clears[HEIGHT];
	for (int i = 0; i < HEIGHT; ++i)
		clears[i] = row(i);

	for (int i = HEIGHT - 1, k = 0; i >= 0; --i)
	{
		while (clears[i - k])
			++k;
		for (int j = 0; j < WIDTH; ++j)
			board[i][j] = i - k < 0 ? 0 : board[i - k][j];
	}
}

static void write()
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (SHAPE[i][j])
				board[y + i][x + j] = SHAPE[i][j];
	clear();
}

int main(void)
{
	InitWindow(800, 600, "tetris ai");
	SetTargetFPS(60);

	next_shape = random_shape();
	bool running = true;
	bool spawn = true;
	int last_drop = 0;
	int frames = 0;

	while (!WindowShouldClose())
	{
		if (!running)
			goto render;
		if (spawn)
		{
			running &= spawn_shape(&x, &y);
			spawn = false;
		}
		if (frames - last_drop == 15)
		{
			if (!move(0, 1, 0))
			{
				write();
				spawn = true;
			}
			last_drop = frames;
		}

		// TODO: DAS
		int key;
		while (key = GetKeyPressed())
			switch (key)
			{
				case KEY_A:
					move(-1, 0, 0);
					break;
				case KEY_S:
					move(0, 1, 0);
					break;
				case KEY_D:
					move(1, 0, 0);
					break;
				case KEY_Q:
					move(0, 0, 1);
					break;
				case KEY_E:
					move(0, 0, -1);
					break;
			}

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
		DrawText(TextFormat("running:\t%s\npos:\t%d %d %d",
		                    running ? "true" : "false",
		                    x, y, r),
		         400, 100, 20, ORANGE);

		for (int k = 0; k < N_SHAPES; ++k)
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					draw_cell(WIDTH + 1 + k % 4 * 5 + j, 13 + i + k / 4 * 5, shapes[k][0][i][j]);
		EndDrawing();
		
		++frames;
	}
	return 0;
}
