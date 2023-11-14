#include "tetris.c"

static int eval(board_t board)
{
	int holes = board_holes(board);
	int height = board_height(board);
	return 2 * (1 + holes) + height;
}

#include "search.c"

int main(void)
{
	init();
	current_shape = 4;

	print_board(board);

	struct node result = {0};
	bfs(board, 0, &result);

	struct node zero = {0};
	assert(0 != memcmp(result, zero, sizeof(struct node)));

	write(board, current_shape, result.x, result.y, result.r);
	print_board(board);

	return 0;
}
