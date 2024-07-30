unsigned char shape_queue[20000];

struct state {
	board_t board;
	int level;
	int lines, points;
	int shape, next_shape;

	int x, y, r;
	int frames;
	int last_input;
	bool spawn;
	int shape_queue_i;
};

static void print_state(struct state *s)
{
	printf("level: %d, lines: %d, shape: %d\n", s->level, s->lines, s->shape);
	printf("x: %d, y: %d, r: %d\n", s->x, s->y, s->r);
	print_board(s->board);
}

static int points_per_line[] = {0, 40, 100, 300, 1200};

static unsigned short random_int(unsigned short *rng)
{
	unsigned short x = 1 & *rng >> 9;
	unsigned short y = 1 & *rng >> 1;
	unsigned short left = x ^ y;
	return *rng = left << 15 | *rng >> 1;;
}

static int random_shape(unsigned short *rng)
{
	static thread_local int last = -1;
	static thread_local int shapes = 0;
	static thread_local int spawn_id[N_SHAPES] = {0x12, 0x0a, 0x0b, 0x08, 0x07, 0x0e, 0x02};

	unsigned char i = random_int(rng) >> 8;
	i += (unsigned char)++shapes;
	i &= 7;
	if (7 == i || i == (unsigned char)last)
	{
		i = random_int(rng) >> 8;
		i &= 7;
		i += (unsigned char)last;
		i %= 7;
	}
	return last = i;
}

static bool spawn_shape(struct state *s)
{
	s->shape = s->next_shape;
	s->shape_queue_i += 1;
	s->shape_queue_i %= 20000;
	s->next_shape = shape_queue[s->shape_queue_i];

	s->x = 3;
	s->y = 1;
	s->r = 0;
	s->frames = 0;
	return !collides(s->board, s->shape, s->x, s->y, s->r);
}

static void inc_level(struct state *s)
{
	if (s->lines >= s->level * 10 + 10 ||
	    s->lines >= 100 && s->lines >= s->level * 10 - 50)
		s->level += s->lines % 10 == 0;
}

static bool move(struct state *s, int dx, int dy, int dr)
{
	if (collides(s->board, s->shape, s->x + dx, s->y + dy, (s->r + dr + 4) % 4)) {
		if (dy) {
			int lines = write(s->board, s->shape, s->x, s->y, s->r);
			s->points += (1 + s->level) * points_per_line[lines];
			s->lines += lines;
			inc_level(s);
			s->spawn = true;
		}
		return false;
	}
	s->x += dx;
	s->y += dy;
	s->r += dr + 4;
	s->r %= 4;
	return true;
}

static void init(struct state *s)
{
	unsigned short seed = (unsigned short)thread_rand();
	for (int i = 0; i < 20000; ++i)
		shape_queue[i] = (char)random_shape(&seed);
	s->next_shape = shape_queue[0];
	spawn_shape(s);
}

static int drop_speed(int l)
{
	switch (l) {
		case 0:
			return 48;
		case 1:
			return 43;
		case 2:
			return 38;
		case 3:
			return 33;
		case 4:
			return 28;
		case 5:
			return 23;
		case 6:
			return 18;
		case 7:
			return 13;
		case 8:
			return 8;
		case 9:
			return 6;
		case 10:
		case 11:
		case 12:
			return 5;
		case 13:
		case 14:
		case 15:
			return 4;
		case 16:
		case 17:
		case 18:
			return 3;
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
			return 2;
		default:
			return 1;;
	}
}

enum input {
	UP      = 0x01,
	DOWN    = 0x02,
	LEFT    = 0x04,
	RIGHT   = 0x08,
	ROT_CW  = 0x10,
	ROT_ACW = 0x20,
};

enum event {
	WRITE = 0x01,
	END   = 0x02,
};

// advance the game by one frame
static int tick(struct state *s, int input)
{
	// random_int();
	++s->frames;

	if (s->spawn) {
		if (!spawn_shape(s))
			return END;
		s->spawn = false;
		s->frames = 0;
	}

	// rotate clockwise / anti clockwise
	switch (input & ~s->last_input & (ROT_CW | ROT_ACW)) {
		case ROT_CW:
			move(s, 0, 0, -1);
			break;
		case ROT_ACW:
			move(s, 0, 0, 1);
			break;
		default:
			break;
	}

	// gravity
	// TODO check that this is correct from the NES assembly
	if (0 == s->frames % drop_speed(s->level))
		if (!move(s, 0, 1, 0))
			return WRITE;

	int dir = input & RIGHT ? 1 : input & LEFT ? -1 : 0;
	// move left / right
	if (input & ~s->last_input & (LEFT | RIGHT))
		move(s, dir, 0, 0);

	s->last_input = input;
	return 0;
}

static int get_dir_x(int dx)
{
	return dx == 1 ? RIGHT : dx == -1 ? LEFT : 0;
}

static int get_dir_r(int dr)
{
	return dr == -1 ? ROT_CW : dr == 1 ? ROT_ACW : 0;
}

static int get_dir(int dx, int dr)
{
	return get_dir_x(dx) | get_dir_r(dr);
}
