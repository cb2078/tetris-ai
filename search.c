#define HASH_SIZE (1 << 6)
#define QUEUE_SIZE (WIDTH * 4)
#define SEARCH_DEPTH 2
#define STATE_COUNT QUEUE_SIZE

static float ALPHA = 0.01f;
static float EPSILON = 0.01f;
static float GAMMA = 0.9f;
static float ITERATIONS = 10000;

static float weights[3] = {0.45f, 0.45f, 0.10f};

static float eval(struct state *s)
{
	return
		weights[0] * board_height(s->board) +
		weights[1] * board_holes(s->board) +
		weights[2] * board_variance(s->board);
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
