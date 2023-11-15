#include "tetris.c"

static int eval(board_t board)
{
	int holes = board_holes(board);
	int height = board_height(board);
	return holes + 5 * height;
}

#include "search.c"

#include "draw.c"

int main(void)
{
	InitWindow(800, 600, "tetris ai");
	SetTargetFPS(90);

	init();

	int len;
	inputs_t inputs;
	search(inputs, &len);

	int i = 0;
	bool running = true;
	bool spawn = false;
	while (!WindowShouldClose())
	{
		if (!running)
			goto render;

		if (spawn)
		{
			spawn = false;
			search(inputs, &len);
			i = 0;
		}

		++frames;
		if (frames >= drop_speed(level))
		{
			frames = 0;
			if (!move(0, 1, 0))
			{
				write(board, current_shape, x, y, r);
				spawn = true;
				running = spawn_shape();
			}
		}
		if (i < len)
		{
			struct input inp = inputs[i];
			assert(move(inp.dx, 0, inp.dr));
			++i;
		}

render:
		draw();
	}
}
