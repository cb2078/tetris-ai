#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define crash() do { *(int *)0 = 200; } while (0)
#define unreachable() crash()
#define assert(cond) if (cond) ; else unreachable();

#include "tetris.c"

static int eval(board_t board, int score)
{
	(void)score;

	int height = board_height(board);
	int holes = board_holes(board);
	float variance = board_variance(board);
	return 5 * holes + (int)variance + 5 * height;
}

#include "search.c"

#include "draw.c"

int main(void)
{
	InitWindow(800, 600, "tetris ai");
	SetTargetFPS(60);

	init();

	int len;
	inputs_t inputs;
	search(inputs, &len);

	int i = 0;
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
				i = 0;
				frames = 0;
			}

			if (i < len && frames == inputs[i].frames - 2)
			{
				struct node *n = &inputs[i];
				assert(move(n->dx, 0, n->dr));
				assert(n->x == x && n->r == r);
				++i;
			}
			++frames;
			int dy = 0 == frames % drop_speed(level);
			if (dy)
			{
				if (!move(0, 1, 0))
				{
					write(board, current_shape, x, y, r);
					if (0 != memcmp(board, board_tmp, sizeof(board_t)))
					{
						puts("board");
						print_board(board);
						puts("\nboard from search");
						print_board(board_tmp);
						crash();
					}
					spawn = true;
					running = spawn_shape();
				}
			}
		}
		draw();
	}
	return 0;
}
