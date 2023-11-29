#include "adt.c"

#define HASH_SIZE	(1 << 6)
#define QUEUE_SIZE	(WIDTH * 4)

struct node {
	int x, y, r;
	int dx, dr;
	int frames;
	struct node *prev;
};

static unsigned node_hash(struct node *n)
{
	// 4 bits for x, 2 bits for r
	return n->x + 4 | n->r << 4;
}

static void print_node(struct node *n)
{
	printf("x %2d\ty %d\tr %d\tdx %2d\tdr %d\tframes %d\n",
			n->x, n->y, n->r,
			n->dx, n->dr,
			n->frames);
}

static int bfs(int depth, board_t board, enum shape_type shape, int points, struct node *nodes, struct node *result)
{
	int best = 1000000;	// TODO should really use int max here

	struct node start = {0};
	start.x = 3;
	start.y = 1;

	bit_set_t visited = bit_set_new(HASH_SIZE);

	struct node *queue_front = nodes;
	struct node *queue_back = nodes;
	*queue_back++ = start;

	while (queue_back  > queue_front) {
		struct node *cur = queue_front++;

		for (int dx = -1; dx <= 1; ++dx)
			for (int dr = -1; dr <= 1; ++dr) {
				struct node next = *cur;
				next.x += next.dx = dx;
				next.r += (next.dr = dr) + 4;
				next.r %= 4;
				next.prev = cur;

				unsigned hash = node_hash(&next);
				if (bit_set_contains(visited, hash))
					continue;
				bit_set_add(visited, hash);

				if (collides(board, shape, next.x, next.y, next.r))
					continue;
				for (int i = 0; i < 2; ++i) {
					next.frames++;
					int dy = next.frames % drop_speed(level) == 0;
					if (dy && collides(board, shape, next.x, next.y + dy, next.r))
						goto lock;
					next.y += dy;
				}

				*queue_back++ = next;
				assert(queue_back - nodes < QUEUE_SIZE);

				while (!collides(board, shape, next.x, next.y + 1, next.r))
					next.y += 1;
lock:
				board_t board_cpy;
				memcpy(board_cpy, board, sizeof(board_t));
				int lines = write(board_cpy, shape, next.x, next.y, next.r);

				int score = 0;
				switch (depth) {
					case 0:
						score = bfs(1 + depth, board_cpy, shape_queue[1],
								points + points_per_line[lines],
								(struct node[QUEUE_SIZE]){0}, result);
						break;
// consider all future shapes in the next box
// this should lead to better placements but at a large performance cost
// not sure if this is worth doing
// a few optimisations need to be made first before this even becomes viable
#if 0
					case 1:
						// TODO use the actual probabilities to calculate the score
						for (enum shape_type s = 0; s < N_SHAPES; ++s)
							if (s != shape)
								score += bfs(1 + depth, board_cpy, s,
										(struct node [QUEUE_SIZE]){0}, result);
						score /= 7;
						break;
					case 2:
						score = eval(board_cpy);
						break;
#else
					case 1:
						score = eval(board_cpy, points);
						break;
#endif
					default:
						unreachable();
				}
				if (score >= best)
					continue;
				best = score;
				if (depth == 0) *result = next;
			}
	}

	assert(best < 1000000);
	return best;
}

board_t board_tmp;
typedef struct node inputs_t[128];
static void search(inputs_t inputs, int *len)
{
	struct node nodes[QUEUE_SIZE];
	struct node result = {0};
	bfs(0, board, shape_queue[0], 0, nodes, &result);
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
