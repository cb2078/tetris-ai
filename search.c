#define HASH_SIZE (1 << 6)
#define QUEUE_SIZE (WIDTH * 4)
#define SEARCH_DEPTH 2
#define STATE_COUNT QUEUE_SIZE

static float ALPHA = 0.01f;
static float EPSILON = 0.01f;
static float GAMMA = 0.9f;
static float ITERATIONS = 10000;

static float weights[3] = {50, 50, 10};
static void q_linear_update(float delta, struct state *s)
{
	weights[0] += ALPHA * delta * board_height(s->board);
	weights[1] += ALPHA * delta * board_holes(s->board);
	weights[2] += ALPHA * delta * board_variance(s->board);

	// normalize weights
	float dot = 0;
	for (int i = 0; i < 3; ++i)
		dot += weights[i] * weights[i];
	float mag = sqrtf(dot);
	for (int i = 0; i < 3; ++i)
		weights[i] = weights[i] / mag * 100;
}

static float q_linear(struct state *s)
{
	return
		weights[0] * board_height(s->board) +
		weights[1] * board_holes(s->board) +
		weights[2] * board_variance(s->board);
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
	return q_linear(s);
}

static void print_state(struct state *s)
{
	printf("level: %d, lines: %d, shape: %d\n", s->level, s->lines, s->shape);
	printf("x: %d, y: %d, r: %d\n", s->last_placement.x, s->last_placement.y, s->last_placement.r);
	print_board(s->board);
}

#define HASH(x, r) ((x) + 4 | (r) << 4)

static void expand(struct state *state, struct state states[STATE_COUNT], unsigned *length)
{
	bit_set_t visited = bit_set_new(HASH_SIZE);
	bit_set_add(visited, HASH(state->x, state->r));

	*length = 0;
	struct state queue[QUEUE_SIZE] = {*state};
	struct state *queue_front = queue;
	struct state *queue_back = queue + 1;
	while (queue_back  > queue_front) {
		struct state *cur = queue_front++;

		for (int dx = -1; dx <= 1; ++dx)
			for (int dr = -1; dr <= 1; ++dr) {
				struct state next = *cur;
				int result;

				// move left / right and rotate
				result = tick(&next, get_dir(dx, dr));
				if (result)
					continue;

				unsigned hash = HASH(next.x, next.r);
				if (bit_set_contains(visited, hash))
					continue;
				bit_set_add(visited, hash);

				// wait 1 frame between inputs
				result = tick(&next, 0);
				if (result & WRITE)
						goto lock;
				if (result & END)
						continue;

				*queue_back++ = next;
				assert(queue_back - queue < QUEUE_SIZE);

				// do a "soft drop"
				while (!(WRITE & tick(&next, 0)));
lock:
				// update the last placement
				next.last_placement.x = next.x;
				next.last_placement.y = next.y;
				next.last_placement.r = next.r;

				states[(*length)++] = next;
			}
	}
}

static struct state *search(struct state *state, struct state states[STATE_COUNT], unsigned depth)
{
	if (depth == 0)
		return state;
	unsigned length;
	expand(state, states, &length);
	float min = 1e9;
	int arg_min = - 1;
	for (unsigned i = 0; i < length; ++i) {
		struct state child_states[STATE_COUNT];
		struct state *next = search(&states[i], child_states, depth - 1);
		if (!next)
			continue;
		float result = eval(next);
		if (result < min) {
			min = result;
			arg_min = i;
		}
	}
	return arg_min == -1 ? NULL : &states[arg_min];
}

static struct state *state_min_q(struct state *state, struct state states[STATE_COUNT])
{
	return search(state, states, SEARCH_DEPTH);
}

static struct state *select_epsilon_greedy(struct state *state, struct state states[STATE_COUNT])
{
	unsigned length;
	expand(state, states, &length);
	if ((float)rand() / RAND_MAX < EPSILON)
		return &states[rand() * length / RAND_MAX];
	return state_min_q(state, states);
}

static struct state *select_epsilon_decreasing(struct state *state, struct state states[STATE_COUNT]);

static void q_learning(void)
{
	for (int i = 0; i < ITERATIONS; ++i) {
		struct state state = { .lines = 19, };
		struct state states[STATE_COUNT];
		for (;;) {
			struct state *new_state = select_epsilon_greedy(&state, states);
			if (!new_state)
				break;
			float reward = (float)points_per_line[new_state->lines];
			float delta = reward + GAMMA * q_linear(new_state) - q_linear(&state);
			q_linear_update(delta, new_state);
			memcpy(&state, new_state, sizeof(struct state));
		}
	}
}
