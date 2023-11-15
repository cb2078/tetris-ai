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

	int len;
	inputs_t inputs;
	search(inputs, &len);

	for (int i = 0; i < len; ++i)
		print_node(inputs + i);

	{
		struct node n = inputs[len - 1];
		write(board, current_shape, n.x, n.y, n.r);
		print_board(board);
	}

	return 0;
}
