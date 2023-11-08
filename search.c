#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 5012

typedef struct Node Node;
struct Node {
	int x, y, r;
	int frames;
	Node *prev; // NOTE if searching is still slow, get rid of this feild
};

typedef struct Queue Queue;
struct Queue {
	int front, back;
	Node buf[BUF_SIZE];	// check if this is a good size
};

static void queue_push(Queue *q, Node *n)
{
	assert((q->front + 1) % BUF_SIZE != q->back);
	q->buf[q->front] = *n;
	q->front = (1 + q->front) % BUF_SIZE;
}

static Node *queue_pop(Queue *q)
{
	assert(q->back != q->front);
	Node *n = &q->buf[q->back];
	q->back = (q->back + 1) % BUF_SIZE;
	return n;
}

static bool queue_empty(Queue *q)
{
	return q->front == q->back;
}

// NOTE change this to a hash set if searching is slow
typedef struct Set Set;
struct Set {
	int front;
	Node buf[BUF_SIZE];
};

static bool set_contains(Set *s, Node *n)
{
	for (int i = 0; i < s->front; ++i)
		// compare all the feilds of Node besides the last one
		if (0 == memcmp(n, &s->buf[i], sizeof(Node) - sizeof(Node *)))
			return true;
	return false;
}

static void set_push(Set *s, Node *n)
{
	assert(s->front < BUF_SIZE);
	s->buf[s->front++] = *n;
}

// TODO extend this to the next box
static void bfs(void)
{
	Node start;
	start.x = x;
	start.y = y;
	start.r = r;
	start.prev = 0;

	Queue q = {0};
	queue_push(&q, &start);

	Set visited = {0};

	while (!queue_empty(&q))
	{
		Node *n = queue_pop(&q);

		// TODO find a more efficient way of checking visited nodes
		if (set_contains(&visited, n))
			continue;
		set_push(&visited, n);

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
				Node child = *n;
				child.x += dx;
				child.y += dy;
				child.r += dr;
				child.r %= 4;
				child.frames = frames;
				child.prev = n;
				if (collides(child.x, child.y, child.r))
					continue;
				queue_push(&q, &child);
			}
	}
}

