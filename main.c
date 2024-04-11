#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

int inputs[HEIGHT * 48];
board_t board_expected;
// NOTE this doesn't work when the placement is very high
static void find_inputs(struct state *state)
{
	struct state states[STATE_COUNT];
	struct state *next = search(state, states, SEARCH_DEPTH);
	memcpy(board_expected, next->board, sizeof(board_t));

	memset(inputs, 0, sizeof(inputs));
	// NOTE this won't work with tucks
	int x = 3, r = 0;
	int dx = next->x > x ? 1 : next->x < x ? -1 : 0;
	int dr = next->r == 0 ? 0 : next->r == 3 ? -1 : 1;
	for (int frame = 0;; frame += 2) {
		bool flag = false;
		if (x != next->x) {
			flag = true;
			inputs[frame] |= get_dir_x(dx);
			x += dx;
		}
		if (r != next->r) {
			flag = true;
			inputs[frame] |= get_dir_r(dr);
			r += dr + 4;
			r %= 4;
		}
		if (!flag)
			break;
	}
}

int main(void)
{
	test_board();

	InitWindow(800, 600, "tetris ai");
	SetTargetFPS(60);

	struct state state = { .level = 18, };
	init(&state);
	find_inputs(&state);

	bool running = true;
	while (running && !WindowShouldClose()) {
		int result = tick(&state, inputs[state.frames]);
		switch (result) {
			case WRITE:
#if 1
				if (0 != memcmp(state.board, board_expected, sizeof(board_t))) {
					puts("board");
					print_board(state.board);
					puts("\nboard from search");
					print_board(board_expected);
					crash();
				}
#endif
				find_inputs(&state);
				break;
			case END:
				running = false;
				break;
		}
		draw(&state);
	}
	return 0;
}
