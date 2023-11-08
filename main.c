#include "tetris.c"
#include "search.c"

int main(void)
{
	init();
	spawn_shape();
	bfs();
	return 0;
}
