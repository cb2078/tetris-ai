#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "adt.c"

#define QUEUE_SIZE 5'000

struct node {
	int x, y, r;
	int frames;
	struct node *prev; // NOTE if searching is still slow, get rid of this feild
};

static bool node_cmp(struct node *x, struct node *y)
{
	return 0 == memcmp(x, y, sizeof(struct node) - sizeof(struct node *));
}

static unsigned short node_hash(struct node *n)
{
	// adding 4 to x ensures it's always positive
	// x needs 4 bits, y needs 5 bits, r needs 2 bits
	// 2^4 * 2^5 * 2^2 * 2^2 = 8192 permutations
	return 4 + n->x | n->y << 4 | n->r << 9 | n->frames << 11;
}

// TODO extend this to the next box
static void bfs(void)
{
	struct node start;
	start.x = x;
	start.y = y;
	start.r = r;
	start.prev = 0;

	struct queue *q = queue_new(QUEUE_SIZE, sizeof(struct node));
	queue_push(q, &start);

	bit_set_t visited = bit_set_new(1 << 13);

	while (!queue_empty(q))
	{
		struct node *n = queue_front(q);
		queue_pop(q);

		int frames = (1 + n->frames) % drop_speed(level);
		int dy = !frames;
		if (collides(n->x, dy + n->y, n->r))
		{
			assert(dy);
#if 1
			x = n->x;
			y = n->y;
			r = n->r;
			printf("%d\n", y);
			write();
			for (int i = 0; i < HEIGHT; ++i)
			{
				for (int j = 0; j < WIDTH; ++j)
					putchar(board[i][j] ? '#' : '.');
				putchar('\n');
			}
			putchar('\n');
			memset(board, 0, sizeof(Board));
#else
			// push the new shape at the spawn position
#endif
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
				child.prev = n;
				if (collides(child.x, child.y, child.r))
					continue;
				unsigned child_hash = node_hash(&child);
				if (bit_set_contains(visited, child_hash))
					continue;
				bit_set_add(visited, child_hash);
				queue_push(q, &child);
			}
	}
}
