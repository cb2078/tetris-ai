#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 1
#define WIN32_LEAN_AND_MEAN
#include <intrin.h>
#define breakpoint() __debugbreak()
#else
#define breakpoint()
#endif

#define crash() do { breakpoint(); *(int *)0 = 200; } while (0)
#define unreachable() crash()
#define assert(cond) if (cond) ; else { printf("assert at %s:%d\n", __FILE__, __LINE__); unreachable(); }

#include "bitboard.c"
#include "adt.c"
#include "tetris.c"
#include "search.c"
#include "draw.c"
#include "test.c"

struct input {
	int dx, dr;
	int frames;
};

board_t board_expected;
// NOTE the inputs array is be sparse
static void find_inputs(struct state *state, struct input inputs[WIDTH], int *len)
{
	struct state states[40];
	struct state *next = search(state, states, SEARCH_DEPTH);
	memcpy(board_expected, next->board, sizeof(board_t));

	memset(inputs, 0, sizeof(struct input) * WIDTH);
	// NOTE this won't work with tucks
	struct node cur = {3, 1, 0, 0};
	int dx = next->last_placement.x > cur.x ? 1 : next->last_placement.x < cur.x ? -1 : 0;
	int dr = next->last_placement.r == 0 ? 0 : next->last_placement.r == 3 ? -1 : 1;
	int flag = true;
	*len = 0;
	for (int frames = 0; flag; ++*len, frames += 2) {
		flag = false;
		if (cur.x != next->last_placement.x) {
			flag = true;
			inputs[*len].dx = dx;
			cur.x += dx;
		}
		if (cur.r != next->last_placement.r) {
			flag = true;
			inputs[*len].dr = dr;
			cur.r += dr + 4;
			cur.r %= 4;
		}
		inputs[*len].frames = frames;
	}
}

int main(void)
{
	test_board();

	InitWindow(800, 600, "tetris ai");
	SetTargetFPS(60);

	init();

	int len;
	struct input inputs[WIDTH];
	find_inputs(&state, inputs, &len);

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
				find_inputs(&state, inputs, &len);
				i = 0;
				frames = 0;
			}

			if (i < len && frames == inputs[i].frames)
			{
				struct input *n = &inputs[i];
				assert(move(&state, n->dx, 0, n->dr));
				// assert(n->x == x && n->r == r);
				++i;
			}
			++frames;
			int dy = 0 == frames % drop_speed(state.level);
			if (dy)
			{
				if (!move(&state, 0, 1, 0))
				{
					int new_lines = write(state.board, state.shape, x, y, r);
					lines += new_lines;
					points += points_per_line[new_lines] * (state.level + 1);
					tetrises += new_lines == 4;

					if (0 != memcmp(state.board, board_expected, sizeof(board_t)))
					{
						puts("board");
						print_board(state.board);
						puts("\nboard from search");
						print_board(board_expected);
						crash();
					}
					spawn = true;
					running = spawn_shape();

				}
			}
		}
		else
			break;
		draw();
	}
	return 0;
}
