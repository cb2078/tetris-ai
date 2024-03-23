#include "raylib.h"

int lines, points, tetrises;

static Color colours[9] = {
	{0, 0, 0, 255},
	{32, 32, 32, 255},
	{48, 48, 48, 255},
	{64, 64, 64, 255},
	{80, 80, 80, 255},
	{96, 96, 96, 255},
	{112, 112, 112, 255},
	{128, 128, 128, 255},
	{16, 16, 16, 255},
};
#define COLOUR_BG colours[8]

static void draw_cell(int x, int y, int c)
{
	static int width = 20;
	DrawRectangle((x + 1) * width, (y - 1) * width, width, width, colours[c]);
}

static void draw(void)
{
	BeginDrawing();
	ClearBackground(COLOUR_BG);

	// board
	for (int i = 2; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			draw_cell(j, i, board_at(state.board, i, j));
	// next box
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			draw_cell(j + WIDTH + 1, i + 2, shape_at(shapes[next_shape][0], i, j));
	// current shape
	{
		int k = y;
		while (k < HEIGHT - 1 && !collides(state.board, state.shape, x, 1 + k, r))
			++k;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				int cell = shape_at(shapes[state.shape][r], i, j);
				if (cell)
				{
					// draw_cell(x + j, k + i, N_SHAPES + 1); // shadow
					draw_cell(x + j, y + i, cell);
				}
			}
	}
	// debug
	DrawText(TextFormat("pos: %d %d %d\nlevel: %d\nlines: %d\npoints %d\ntetris rate %f%", x, y, r, state.level,
				lines, points, (float)tetrises * 4 / lines * 100),
			400, 10, 20, ORANGE);
	for (int k = 0; k < N_SHAPES; ++k)
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				draw_cell(WIDTH + 1 + k % 4 * 5 + j, 13 + i + k / 4 * 5, shape_at(shapes[k][0], i,j));
	EndDrawing();
}
