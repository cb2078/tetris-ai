#define HASH_SIZE	(1 << 6)
#define QUEUE_SIZE	(WIDTH * 4)
#define SEARCH_DEPTH	2

static float ALPHA = 0.01f;
static float EPSILON = 0.01f;
static float GAMMA = 0.9f;
static float ITERATIONS = 10000;

static float weights[5] = {0};
static void q_linear_update(float delta, struct state *s)
{
	weights[0] += ALPHA * delta * board_row_transitions(s->board);
	weights[1] += ALPHA * delta * board_col_transitions(s->board);
	weights[2] += ALPHA * delta * board_wells(s->board);
	weights[3] += ALPHA * delta * board_cell_count(s->board);
	weights[4] += ALPHA * delta * drop_speed(s->level);
}

static float q_linear(struct state *s)
{
	return
		weights[0] * board_row_transitions(s->board) +
		weights[1] * board_col_transitions(s->board) +
		weights[2] * board_wells(s->board) +
		weights[3] * board_cell_count(s->board) +
		weights[4] * drop_speed(s->level);
}

static float simple_eval(struct state *s)
{
       int height = board_height(s->board);
       int holes = board_holes(s->board);
       float variance = board_variance(s->board);
       return 5 * holes + variance + 5 * height;
}

static inline float eval(struct state *s)
{
	return simple_eval(s);
}

static void print_state(struct state *s)
{
	printf("level: %d, lines: %d, shape: %d\n", s->level, s->lines, s->shape);
	printf("x: %d, y: %d, r: %d\n", s->last_placement.x, s->last_placement.y, s->last_placement.r);
	print_board(s->board);
}

struct node {
	int x, y, r;
	int frames;
};

static unsigned node_hash(struct node *n)
{
	// 4 bits for x, 2 bits for r
	return n->x + 4 | n->r << 4;
}

static void print_node(struct node *n)
{
	printf("x %2d\ty %d\tr %d\tframes %d\n", n->x, n->y, n->r, n->frames);
}

static void expand(struct state *state, struct state states[40], unsigned *length)
{
	*length = 0;
	bit_set_t visited = bit_set_new(HASH_SIZE);

	struct node nodes[40] = { [0] = { 3, 1, 0, 0, }, };
	struct node *queue_front = nodes;
	struct node *queue_back = nodes + 1;
	while (queue_back  > queue_front) {
		struct node *cur = queue_front++;

		for (int dx = -1; dx <= 1; ++dx)
			for (int dr = -1; dr <= 1; ++dr) {
				struct node next = *cur;
				next.x += dx;
				next.r += dr + 4;
				next.r %= 4;

				unsigned hash = node_hash(&next);
				if (bit_set_contains(visited, hash))
					continue;
				bit_set_add(visited, hash);

				if (collides(state->board, state->shape, next.x, next.y, next.r))
					continue;
				for (int i = 0; i < 2; ++i) {
					next.frames++;
					int dy = next.frames % drop_speed(state->level) == 0;
					if (dy && collides(state->board, state->shape, next.x, next.y + dy, next.r))
						goto lock;
					next.y += dy;
				}

				*queue_back++ = next;
				assert(queue_back - nodes < QUEUE_SIZE);

				while (!collides(state->board, state->shape, next.x, next.y + 1, next.r))
					next.y += 1;
lock:
#if 0
				memcpy(states[*length].board, state->board, sizeof(board_t));
#else
				memcpy(&states[*length], state, sizeof(struct state));
#endif
				states[*length].lines = write(states[*length].board, state->shape, next.x, next.y, next.r);
				states[*length].last_placement.x = next.x;
				states[*length].last_placement.y = next.y;
				states[*length].last_placement.r = next.r;
				// NOTE this is a hack and should be handled in the write function
				states[*length].shape = next_shape;
				*length += 1;
			}
	}
}

static struct state *search(struct state *state, struct state states[40], unsigned depth)
{
	if (depth == 0)
		return state;
	unsigned length;
	expand(state, states, &length);
	float min = 1e9;
	int arg_min = - 1;
	for (unsigned i = 0; i < length; ++i) {
		struct state child_states[40];
		float result = eval(search(&states[i], child_states, depth - 1));
		if (result < min) {
			min = result;
			arg_min = i;
		}
	}
	assert(arg_min != -1);
	return &states[arg_min];
}

static struct state *state_min_q(struct state *state, struct state states[40])
{
	return search(state, states, SEARCH_DEPTH);
}

static struct state *select_epsilon_greedy(struct state *state, struct state states[40])
{
	unsigned length;
	expand(state, states, &length);
	if ((float)rand() / RAND_MAX < EPSILON)
		return &states[rand() * length / RAND_MAX];
	return state_min_q(state, states);
}

static struct state *select_epsilon_decreasing(struct state *state, struct state states[40]);

static void q_learning(void)
{
	for (int i = 0; i < ITERATIONS; ++i) {
		struct state states[40];
		struct state *state = &states[0]; memset(state, 0, sizeof(struct state));
		while (state) {
			struct state *new_state = select_epsilon_greedy(state, states);
			float reward = (float)points_per_line[new_state->lines];
			float delta = reward + GAMMA * q_linear(new_state) - q_linear(state);
			q_linear_update(delta, new_state);
			state = new_state;
		}
	}
}
