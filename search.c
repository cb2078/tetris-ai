#define HASH_SIZE (1 << 6)
#define QUEUE_SIZE (WIDTH * 4)
#define SEARCH_DEPTH 2
#define STATE_COUNT QUEUE_SIZE

#define POPULATION 40
#define SAMPLES 1
#define EPOCHS 45

static float ALPHA = 0.005f;
static float EPSILON = 0.01f; // not used
static float GAMMA = 0.9f; // not used
static float SIGMA = 0.1f;

#define WEIGHT_COUNT 4
// static float weights[WEIGHT_COUNT] = {0};
static float weights[WEIGHT_COUNT] = {0.129f, 0.480f, 0.185f, 0.261f};

static float eval(float weights[WEIGHT_COUNT], struct state *s)
{
	return
		weights[0] * board_height(s->board) +
		weights[1] * board_holes(s->board) +
		weights[2] * board_variance(s->board) +
		weights[3] * board_cell_count(s->board);
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
	for (int i = 0; i < SAMPLES; ++i) {
		struct state *s0 = &(struct state){ .level = 18, }; init(s0);
		for (;;) {
			// if (s0->lines > 10000)
			if (s0->level > 40)
				break;
			// draw(s0);
			struct state states[STATE_COUNT];
			struct state *s1 = search(weights, s0, states, SEARCH_DEPTH);
			if (!s1)
				break;
			memcpy(s0, s1, sizeof(struct state));
		}
#if 0
		result += s0->lines;
#else
		result += s0->points;
#endif
	}
	return result / SAMPLES;
}

void es_agent(float N[POPULATION][WEIGHT_COUNT], float R[POPULATION], int i)
{
	float w[WEIGHT_COUNT];
	for (int j = 0; j < WEIGHT_COUNT; ++j)
		w[j] = weights[j] + SIGMA * N[i][j];
	R[i] = (float)run(w);
	fprintf(stderr, ".");
}

struct es_agent_arg {
	struct work_queue queue;
	float (*N)[WEIGHT_COUNT];
	float *R;
	unsigned short seed;
};

int es_agent_parallel(void *x)
{
	struct es_agent_arg *arg = (struct es_agent_arg *)x;
	while (arg->queue.next < arg->queue.count) {
		int i = atomic_fetch_add(&(_Atomic int)arg->queue.next, 1);
		assert(i < arg->queue.next);

		es_agent(arg->N, arg->R, i);
	}
	return 0;
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

#if 1
	struct es_agent_arg arg = {
		.queue = { .count = POPULATION },
		.N = N, .R = R, .seed = (unsigned short)rand(),
	};
	run_jobs(es_agent_parallel, (void *)&arg);
#else
	for (int i = 0; i < POPULATION; ++i)
		es_agent(N, R, i);
#endif
	printf("\t{'lines': [");
	float avg = 0;
	for (int i = 0; i < POPULATION; ++i) {
		avg += R[i] / POPULATION;
		printf("%s%4d", i ? ", " : "", (int)R[i]);
	}
	printf("], 'weights': [");
	for (int j = 0; j < WEIGHT_COUNT; ++j)
		printf("%s%1.3f", j ? ", " : "", weights[j]);
	printf("]},\n");
	fprintf(stderr, " avg %4.1f\n", avg);

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
}

static void improve()
{
	printf("[\n");
	for (int i = 0; i < EPOCHS; ++i) {
		fprintf(stderr, "epoch %3d", i);
		es_iteration();
	}
	printf("]\n");
}
