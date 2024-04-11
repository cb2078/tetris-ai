#define HASH_SIZE (1 << 6)
#define QUEUE_SIZE (WIDTH * 4)
#define SEARCH_DEPTH 2
#define STATE_COUNT QUEUE_SIZE

#define POPULATION 50
static int ITERATIONS = 300;

static float ALPHA = 0.001f;
static float EPSILON = 0.001f;
static float GAMMA = 0.9f;
static float SIGMA = 0.1f;

#define WEIGHT_COUNT 3
// {0.164264f, 0.288453f, 0.111142f} learned from ES
static float weights[WEIGHT_COUNT] = { [0] = 1, };

static float eval(float weights[WEIGHT_COUNT], struct state *s)
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

static struct state *
search(float weights[WEIGHT_COUNT], struct state *state, struct state states[STATE_COUNT], unsigned depth)
{
	if (depth == 0)
		return state;
	unsigned length;
	expand(state, states, &length);
	float min = 1e9;
	int arg_min = - 1;
	for (unsigned i = 0; i < length; ++i) {
		struct state child_states[STATE_COUNT];
		struct state *next = search(weights, &states[i], child_states, depth - 1);
		if (!next)
			continue;
		float result = eval(weights, next);
		if (result < min) {
			min = result;
			arg_min = i;
		}
	}
	return arg_min == -1 ? NULL : &states[arg_min];
}

static int run(float weights[WEIGHT_COUNT])
{
	int result = 0;
	for (int i = 0; i < 5; ++i) {
		struct state *s0 = &(struct state){ .level = 30, }; init(s0);
		for (;;) {
			if (s0->lines > 1000)
				break;
			// draw(s0);
			struct state states[STATE_COUNT];
			struct state *s1 = search(weights, s0, states, SEARCH_DEPTH);
			if (!s1)
				break;
			memcpy(s0, s1, sizeof(struct state));
		}
		result += s0->lines;
	}
	return result / 5;
}

// do 1 iteration
static void es_iteration(void)
{
	// random population of weights
	float N[POPULATION][WEIGHT_COUNT];
	for (int i = 0; i < POPULATION; ++i)
		for (int j = 0; j < WEIGHT_COUNT; ++j)
			N[i][j] = randn();
	float R[POPULATION];

	int best = 0;
	for (int i = 0; i < POPULATION; ++i) {
		printf("%d ", i);
		float w[WEIGHT_COUNT];
		for (int j = 0; j < WEIGHT_COUNT; ++j)
			w[j] = weights[j] + SIGMA * N[i][j];
		R[i] = (float)run(w);
		if ((int)R[i] > best)
			best = (int)R[i];
	}
	putchar('\t');

	float R_mean = 0;
	for (int i = 0; i < POPULATION; ++i)
		R_mean += R[i];
	R_mean /= POPULATION;

	float R_std = 0;
	for (int i = 0; i < POPULATION; ++i)
		R_std += powf(R[i] - R_mean, 2);
	R_std /= POPULATION;
	R_std = sqrtf(R_std);

	// standardize the rewards using a normal distribution
	float A[POPULATION];
	for (int i = 0; i < POPULATION; ++i)
		A[i] = (R[i] - R_mean) / R_std;

	// updat the weights
	float dots[WEIGHT_COUNT] = {0};
	for (int i = 0; i < POPULATION; ++i)
		for (int j = 0; j < WEIGHT_COUNT; ++j)
			dots[j] += N[i][j] * A[i];
	for (int j = 0; j < WEIGHT_COUNT; ++j)
		weights[j] = weights[j] + ALPHA / (POPULATION * SIGMA) * dots[j];

	for (int j = 0; j < WEIGHT_COUNT; ++j)
		printf("% f ", weights[j]);
	printf("\tbest: %d\n", best);
}

static void improve()
{
	for (int i = 0; i < ITERATIONS; ++i)
		es_iteration();
}
