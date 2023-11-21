#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "adt.c"

struct node {
	int x, y, r;
	int dx, dr;
	int frames;
	struct node *prev;
};

// x needs 4 bits, r needs 2 bits
#define HASH_SIZE	(1 << 6)
static unsigned node_hash(struct node *n)
{
	return n->x + 4 | n->r << 4;
}

static void print_node(struct node *n)
{
	printf("x %2d\ty %d\tr %d\tdx %2d\tdr %d\tframes %d\n",
			n->x, n->y, n->r,
			n->dx, n->dr,
			n->frames);
}

static int bfs(board_t board, int shape_index, struct node *nodes, struct node *result)
{
	int shape = shape_queue[shape_index];

	struct node start = {0};
	start.x = 3;
	start.y = 1;

	bit_set_t visited = bit_set_new(HASH_SIZE);
	bit_set_add(visited, node_hash(&start));

	struct node *queue_front = nodes;
	struct node *queue_back = nodes;
	*queue_back++ = start;

	while (queue_back  > queue_front) {
		struct node *cur = queue_front++;

		int frames = 1 + cur->frames;
		int dy = frames % drop_speed(level) == 0;

		for (int dx = -1; dx <= 1; ++dx)
			for (int dr = -1; dr <= 1; ++dr) {
				struct node next = *cur;
				next.x += next.dx = dx;
				next.y += dy;
				next.r += (next.dr = dr) + 4;
				next.r %= 4;
				next.frames = frames;
				next.prev = cur;

				unsigned hash = node_hash(&next);
				if (bit_set_contains(visited, hash))
					continue;
				bit_set_add(visited, hash);

				if (collides(board, shape, next.x, next.y, next.r))
					continue;

				*queue_back++ = next;
			}
	}

	board_t board_cpy;
	int len = (int)(queue_back - nodes);
	assert(len < 40);
	int best = 1000000;
	for (int i = 0; i < len; ++i) {
		struct node n = nodes[i];
		while (!collides(board, shape, n.x, n.y + 1, n.r))
			n.y += 1;
		memcpy(board_cpy, board, sizeof(board_t));
		write(board_cpy, shape, n.x, n.y, n.r);

		int score = shape_index == 1 ? eval(board_cpy) :
			bfs(board_cpy, 1, (struct node[40]){0}, result);
		if (score >= best)
			continue;
		best = score;
		if (shape_index == 0) *result = n;
	}
	assert(best < 1000000);
	return best;
}

board_t board_tmp;
typedef struct node inputs_t[128];
static void search(inputs_t inputs, int *len)
{
	struct node nodes[40];
	struct node result = {0};
	bfs(board, 0, nodes, &result);
	memcpy(board_tmp, board, sizeof(board_t));
	write(board_tmp, current_shape, result.x, result.y, result.r);

	int i = 128;
	for (struct node *n = &result; n; n = n->prev)
		if (n->dx || n->dr)
			inputs[--i] = *n;
	*len = 128 - i;
	assert(len >= 0);
	memmove(inputs, inputs + i, sizeof(struct node) * *len);
}
