#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct Node Node;
struct Node {
	int x, y, r;
	int frames;
	Node *prev; // NOTE if searching is still slow, get rid of this feild
};

#define QUEUE_SIZE 5'000
typedef struct Queue Queue;
struct Queue {
	int front, back;
	Node buf[QUEUE_SIZE];
};

static void queue_push(Queue *q, Node *n)
{
	assert(q->front < QUEUE_SIZE);
	q->buf[q->front] = *n;
	++q->front;
}

static Node *queue_pop(Queue *q)
{
	assert(q->back < q->front);
	Node *n = &q->buf[q->back];
	++q->back;
	return n;
}

static bool queue_empty(Queue *q)
{
	return q->front == q->back;
}

#define SET_SIZE 5'000
// NOTE change this to a hash set if searching is slow
typedef struct Set Set;
struct Set {
	int front;
	Node buf[SET_SIZE];
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
	assert(s->front < SET_SIZE);
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
				// TODO find a more efficient way of checking visited nodes
				if (set_contains(&visited, &child))
					continue;
				set_push(&visited, &child);
				queue_push(&q, &child);
			}
	}
}

