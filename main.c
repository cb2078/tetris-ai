#include "tetris.c"
#include "search.c"

int main(void)
{
	init();
	shape = 4;

	struct vec *results = vec_new_heap(2000, sizeof(board_t));
	bfs(board, shape, results);

	int len = results->len;
	for (int i = 0; i < len; ++i)
		bfs(vec_at(results, i), next_shape, results);

	printf("%d %d\n", len, results->len);

	return 0;
}
