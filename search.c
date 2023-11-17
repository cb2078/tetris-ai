#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "adt.c"

struct node {
	int x, y, r;
	int dx, dy, dr;
	int frames;
	struct node *prev;
};

static bool node_cmp(struct node *x, struct node *y)
{
	bool result = x->x == y->x &&
		x->y == y->y &&
		x->r == y->r &&
		x->frames == y->frames;
	return result;
}

static unsigned node_hash(struct node *n)
{
	// adding 4 to x ensures it's always positive
	// x needs 4 bits, y needs 5 bits, r needs 2 bits
	// 2^4 * 2^5 * 2^2 * 2^2 = 8192 permutations
	return 4 + n->x | n->y << 4 | n->r << 9 | n->frames << 11;
}

static void print_node(struct node *n)
{
	printf("x %2d\ty %d\tr %d\tdx %2d\tdr %d\tframes %d\n",
			n->x, n->y, n->r,
			n->dx, n->dr,
			n->frames);
}

static int bfs(board_t board, int shape_index, struct node *result, struct queue *q)
{
	int best = 1000000;

	int shape = shape_queue[shape_index];

	board_t board_orig;
	memcpy(board_orig, board, sizeof(board_t));

	struct node start = {0};
	start.x = 3;
	start.y = 1;

	if (!q)
		q = queue_new(5000, sizeof(struct node));
	queue_push(q, &start);

	bit_set_t visited = bit_set_new(1 << 13);

	while (!queue_empty(q))
	{
		struct node *n = queue_front(q);
		queue_pop(q);

		int frames = (1 + n->frames) % drop_speed(level);
		int dy = !frames;
		if (collides(board, shape, n->x, dy + n->y, n->r))
		{
			assert(dy);
			write(board, shape, n->x, n->y, n->r);
			int score = shape_index == 1 ? eval(board) :
				bfs(board, 1, result, 0);
			memcpy(board, board_orig, sizeof(board_t));
			if (score >= best)
				continue;
			best = score;
			if (shape_index == 0)
				*result = *n;
			continue;
		}
		// TODO 30hz tapping instead of 60hz
		for (int dx = -1; dx <= 1; ++dx)
			for (int dr = 3; dr <= 5; ++dr)
			{
				struct node child = *n;
				child.x += child.dx = dx;
				child.y += child.dy = dy;
				child.r += child.dr = dr;
				child.r %= 4;
				child.frames = frames;
				child.prev = n;
				if (collides(board, shape, child.x, child.y, child.r))
					continue;
				unsigned child_hash = node_hash(&child);
				if (bit_set_contains(visited, child_hash))
					continue;
				bit_set_add(visited, child_hash);
				queue_push(q, &child);
			}
	}
	assert(1000000 > best);
	return best;
}

typedef struct node inputs_t[128];
static void search(inputs_t inputs, int *len)
{
	struct node result = {0};
	struct queue *q = queue_new(5000, sizeof(struct node));
	bfs(board, 0, &result, q);

	int i = 128;
	for (struct node *n = &result; n; n = n->prev)
		inputs[--i] = *n;
	*len = 128 - i;
	memmove(inputs, inputs + i, sizeof(struct node) * *len);
}
