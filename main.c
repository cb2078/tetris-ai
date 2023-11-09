#include "tetris.c"
#include "search.c"

int main(void)
{
	init();
	shape = 4;

	struct vec *results = vec_new(2000, sizeof(struct pos));
	bfs(shape, results);

	board_t cpy;
	memcpy(cpy, board, sizeof(board_t));
	int len = results->len;
	for (int i = 0; i < len; ++i)
	{
		struct pos *p = (struct pos *)results->buf + i;
		x = p->x;
		y = p->y;
		r = p->r;

		write(shape);
		bfs(next_shape, results);
		memcpy(board, cpy, sizeof(board_t));
	}

	printf("%d %d\n", len, results->len);

	return 0;
}
