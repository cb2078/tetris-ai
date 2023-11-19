#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "adt.c"

#define QUEUE_SIZE	5000

struct node {
	int x, y, r;
	int dx, dy, dr;
	int frames;
	struct node *prev;
};

static unsigned node_hash(struct node *n)
{
	// adding 4 to x ensures it's always positive
	// x needs 4 bits, y needs 5 bits, r needs 2 bits
	// 2^4 * 2^5 * 2^2 * 2^2 = 8192 permutations
	return 4 + n->x | n->y << 4 | n->r << 9 | (n->frames % drop_speed(level)) << 11;
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

	assert(!collides(board, shape_index, start.x, start.y, start.r));

	if (!q)
		q = queue_new(QUEUE_SIZE, sizeof(struct node));
	queue_push(q, &start);

	bit_set_t visited = bit_set_new(1 << 13);

	while (!queue_empty(q))
	{
		struct node *n = queue_front(q);
		queue_pop(q);

		int frames = 1 + n->frames;
		int dy = 0 == frames % drop_speed(level);

		// TODO 30hz tapping instead of 60hz
		int dirs[] = {0, -1, 1};
		for (int x = 0; x < 3; ++x)
			for (int r = 2; r >= 0; --r)
			{
				struct node child = *n;
				child.prev = n;
				child.frames = frames;
				child.x += child.dx = dirs[x];
				child.r += (child.dr = dirs[r]) + 4;
				child.r %= 4;

				unsigned child_hash = node_hash(&child);
				if (bit_set_contains(visited, child_hash))
					continue;
				bit_set_add(visited, child_hash);

				if (collides(board, shape, child.x, child.y, child.r))
					continue;

				// TODO get rid of the duplicated code
				child.y += child.dy = dy;
				if (dy && collides(board, shape, child.x, child.y, child.r))
				{
					if (!collides(board, shape, n->x, n->y + 1, n->r))
						continue;
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

				if (dirs[x] == 0 && n->dx == 0 || dirs[r] == 0 && n->dr == 0)
				{
					while (!collides(board, shape, child.x, child.y + 1, child.r))
					{
						bit_set_add(visited, node_hash(&child));
						child.y += 1;
					}
					write(board, shape, child.x, child.y, child.r);

					int score = shape_index == 1 ? eval(board) :
						bfs(board, 1, result, 0);
					memcpy(board, board_orig, sizeof(board_t));
					if (score >= best)
						continue;
					best = score;
					if (shape_index == 0)
						*result = child;
					continue;
				}

				queue_push(q, &child);
			}
	}
	assert(1000000 > best);
	return best;
}

board_t board_tmp;
typedef struct node inputs_t[128];
static void search(inputs_t inputs, int *len)
{
	struct node result = {0};
	struct queue *q = queue_new(QUEUE_SIZE, sizeof(struct node));
	bfs(board, 0, &result, q);
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
