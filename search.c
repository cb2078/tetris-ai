#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "adt.c"

#define QUEUE_SIZE 5'000

struct node {
	int x, y, r;
	int frames;
};

static bool node_cmp(struct node *x, struct node *y)
{
	return 0 == memcmp(x, y, sizeof(struct node));
}

static unsigned short node_hash(struct node *n)
{
	// adding 4 to x ensures it's always positive
	// x needs 4 bits, y needs 5 bits, r needs 2 bits
	// 2^4 * 2^5 * 2^2 * 2^2 = 8192 permutations
	return 4 + n->x | n->y << 4 | n->r << 9 | n->frames << 11;
}

int c;

// TODO extend this to the next box
static void bfs(board_t board, int shape, struct vec *results)
{
	board_t board_orig;
	memcpy(board_orig, board, sizeof(board_t));

	struct node start;
	start.x = 3;
	start.y = 1;
	start.r = 0;

	struct queue *q = queue_new(QUEUE_SIZE, sizeof(struct node));
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
			vec_push(results, board);
			memcpy(board, board_orig, sizeof(board_t));
			continue;
		}
		// TODO 30hz tapping instead of 60hz
		for (int dx = -1; dx <= 1; ++dx)
			for (int dr = 3; dr <= 5; ++dr)
			{
				struct node child = *n;
				child.x += dx;
				child.y += dy;
				child.r += dr;
				child.r %= 4;
				child.frames = frames;
				if (collides(board, shape, child.x, child.y, child.r))
					continue;
				unsigned child_hash = node_hash(&child);
				if (bit_set_contains(visited, child_hash))
					continue;
				bit_set_add(visited, child_hash);
				queue_push(q, &child);
			}
	}
}
