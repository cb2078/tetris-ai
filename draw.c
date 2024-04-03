#include "raylib.h"

static Color colours[9] = {
	{0, 0, 0, 255},
	{255, 255, 255, 255},
	{223, 223, 223, 255},
	{191, 191, 191, 255},
	{159, 159, 159, 255},
	{127, 127, 127, 255},
	{95, 95, 95, 255},
	{63, 63, 63, 255},
	{31, 31, 31, 255},
};
#define COLOUR_BG colours[8]

static void draw_cell(int x, int y, int c)
{
	static int width = 20;
	DrawRectangle((x + 1) * width, (y - 1) * width, width, width, colours[c]);
}

static void draw(struct state *s)
{
	BeginDrawing();
	ClearBackground(COLOUR_BG);

	// board
	for (int i = 2; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			draw_cell(j, i, board_at(s->board, i, j));
	// next box
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j) {
			draw_cell(j + WIDTH + 1, i + 2, shape_at(shapes[s->next_shape][0], i, j));
			draw_cell(j + WIDTH + 1, i + 7, shape_at(shapes[shape_queue[s->shape_queue_i + 2]][0], i, j));
		}
	// current shape
	{
		int k = s->y;
		while (k < HEIGHT - 1 && !collides(s->board, s->shape, s->x, 1 + k, s->r))
			++k;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
			{
				int cell = shape_at(shapes[s->shape][s->r], i, j);
				if (cell)
				{
					// draw_cell(x + j, k + i, N_SHAPES + 1); // shadow
					draw_cell(s->x + j, s->y + i, cell);
				}
			}
	}
	// debug
	DrawText(TextFormat("pos: %d %d %d\nlevel: %d\nlines: %d\npoints %d", s->x, s->y, s->r, s->level,
				s->lines, s->points),
			400, 10, 20, ORANGE);
	for (int k = 0; k < N_SHAPES; ++k)
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				draw_cell(WIDTH + 1 + k % 4 * 5 + j, 13 + i + k / 4 * 5, shape_at(shapes[k][0], i,j));
	EndDrawing();
}
