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

	int i = 1;
	bool running = true;
	bool spawn = false;
	while (!WindowShouldClose())
	{
		if (running)
		{

			if (spawn)
			{
				spawn = false;
				search(inputs, &len);
				i = 1;
				frames = 0;
			}

			++frames;
			int dy = 0 == frames % drop_speed(level);
			if (i < len)
			{
				struct node *n = &inputs[i];
				assert(move(n->dx, 0, n->dr));
				assert(n->x == x && n->y == dy + y && n->r == r && n->frames == frames);
				++i;
			}
			if (dy)
			{
				if (!move(0, 1, 0))
				{
					write(board, current_shape, x, y, r);
					spawn = true;
					running = spawn_shape();
				}
			}
		}
		draw();
	}
}
