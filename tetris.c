int x, y, r;
int frames = 0;
int next_shape;

struct state {
	board_t board;
	int level;
	int lines;
	int shape;
	struct { int x, y, r; } last_placement;
};

struct state state = {
	.level = 19,
};

static int points_per_line[] = { 0, 40, 100, 300, 1200, };

static unsigned short random_int(void)
{
	static unsigned short seed = 2;
	unsigned short x = 1 & seed >> 9;
	unsigned short y = 1 & seed >> 1;
	unsigned short left = x ^ y;
	seed = left << 15 | seed >> 1;
	return seed;
}

static int random_shape(void)
{
	static int shapes = 0;
	static int spawn_id[N_SHAPES] = { 0x12, 0x0a, 0x0b, 0x08, 0x07, 0x0e, 0x02, };

	unsigned char i = random_int() >> 8;
	i += (unsigned char)++shapes;
	i &= 7;
	if (7 == i || i == state.shape)
	{
		i = random_int() >> 8;
		i &= 7;
		i += (unsigned char)state.shape;
		i %= 7;
	}
	return i;
}

static bool spawn_shape(void)
{
	state.shape = next_shape;
	next_shape = random_shape();
	x = 3;
	y = 1;
	r = 0;
	frames = 0;
	return !collides(state.board, state.shape, x, y, r);
}

static void inc_level(struct state *s)
{
	if (s->lines >= s->level * 10 + 10 ||
	    s->lines >= 100 && s->lines >= s->level * 10 - 50)
		s->level += s->lines % 10 == 0;
}

static bool move(struct state *s, int dx, int dy, int dr)
{
	if (collides(s->board, s->shape, x + dx, y + dy, (r + dr + 4) % 4))
		return false;
	x += dx;
	y += dy;
	r = (r + dr + 4) % 4;
	return true;
}

static void init()
{
	next_shape = random_shape();
	spawn_shape();
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
