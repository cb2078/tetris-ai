#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "adt.c"

#define QUEUE_SIZE	3000
#define BIT_SET_SIZE	(1 << 13)

struct pos {
	int x, y, r;
	int frames;
};

static bool pos_cmp(struct pos *x, struct pos *y)
{
	bool result = x->x == y->x &&
		x->y == y->y &&
		x->r == y->r &&
		x->frames == y->frames;
	return result;
}

static unsigned pos_hash(struct pos *p)
{
	// adding 4 to x ensures it's always positive
	// x needs 4 bits, y needs 5 bits, r needs 2 bits
	// 2^4 * 2^5 * 2^2 * 2^2 = 8192 permutations
	return 4 + p->x | p->y << 4 | p->r << 9 | p->frames << 11;
}

static void print_pos(struct pos *p)
{
	printf("x %2d\ty %d\tr %d\tframes %d\n",
			p->x, p->y, p->r, p->frames);
}

static int bfs(board_t board, int shape_index, struct pos *result)
{
	int best = 1000000;

	int shape = shape_queue[shape_index];

	board_t board_orig;
	memcpy(board_orig, board, sizeof(board_t));

	struct pos start = {0};
	start.x = 3;
	start.y = 1;

	struct queue *q = queue_new(QUEUE_SIZE, sizeof(struct pos));
	queue_push(q, &start);

	bit_set_t visited = bit_set_new(BIT_SET_SIZE);

	while (!queue_empty(q))
	{
		struct pos *p = queue_front(q);
		queue_pop(q);

		int frames = (1 + p->frames) % drop_speed(level);
		int dy = !frames;
		if (collides(board, shape, p->x, dy + p->y, p->r))
		{
			assert(dy);
			write(board, shape, p->x, p->y, p->r);
			int score = shape_index == 1 ? eval(board) :
				bfs(board, 1, result);
			memcpy(board, board_orig, sizeof(board_t));
			if (score >= best)
				continue;
			best = score;
			if (shape_index == 0)
				*result = *p;
			continue;
		}
		// TODO 30hz tapping instead of 60hz
		for (int dx = -1; dx <= 1; ++dx)
			for (int dr = 3; dr <= 5; ++dr)
			{
				struct pos child = *p;
				child.x += dx;
				child.y += dy;
				child.r += dr;
				child.r %= 4;
				child.frames = frames;

				if (collides(board, shape, child.x, child.y, child.r))
					continue;
				unsigned child_hash = pos_hash(&child);
				if (bit_set_contains(visited, child_hash))
					continue;
				bit_set_add(visited, child_hash);
				queue_push(q, &child);
			}
	}
	assert(1000000 > best);
	return best;
}

struct input {
	int dx, dr, frames;
};

typedef struct input inputs_t[128];

static void find_inputs(struct pos *dst, inputs_t inputs, int *len)
{
	struct node {
		struct pos pos;
		struct node *next;
	};
	struct node end = { *dst, 0, };

	struct queue *q = queue_new(QUEUE_SIZE, sizeof(struct node));
	queue_push(q, &end);

	bit_set_t visited = bit_set_new(BIT_SET_SIZE);

	while (!queue_empty(q))
	{
		struct node *n = queue_front(q);
		queue_pop(q);
		assert(n->pos.y > 0);

		bool match = n->pos.x == 3 &&
			n->pos.y == 1 &&
			n->pos.r == 0 &&
			n->pos.frames == frames;
		if (match)
		{
			*len = 0;
			for (; n->next; n = n->next)
			{
				struct input input;
				input.dx = n->next->pos.x - n->pos.x;
				input.dr = n->next->pos.r - n->pos.r;
				input.frames = n->pos.frames;
				inputs[*len] = input;
				*len += 1;
			}
			return;
		}

		int frames = (n->pos.frames ? n->pos.frames : drop_speed(level)) - 1;
		int dy = !frames;
		int dxs[3] = {0, -1, 1};
		int drs[3] = {4,  5, 3};
		// TODO 30hz tapping instead of 60hz
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
			{
				struct node child = *n;
				child.pos.x += dxs[i];
				child.pos.y -= dy;
				child.pos.r += drs[j]; child.pos.r %= 4;
				child.pos.frames = frames;
				child.next = n;

				if (collides(board, current_shape, child.pos.x, child.pos.y, child.pos.r))
					continue;
				unsigned child_hash = pos_hash(&child.pos);
				if (bit_set_contains(visited, child_hash))
					continue;
				bit_set_add(visited, child_hash);
				queue_push(q, &child);
			}
	}
}

static void search(inputs_t inputs, int *len)
{
	struct pos dst;
	bfs(board, 0, &dst);
	find_inputs(&dst, inputs, len);
}
