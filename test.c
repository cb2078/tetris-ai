#if !defined(BOARD)
static void test_board_collision(void)
{
	board_t board = {0};
	int x = 3;
	int y = 12;
	int r = 0;

	write(board, SHAPE_O, x, 12, r);
	// print_board(board);
	assert(collides(board, SHAPE_O, x, y, r));
	assert(collides(board, SHAPE_O, x + 1, y, r));
	assert(collides(board, SHAPE_O, x - 1, y, r));
	assert(collides(board, SHAPE_O, x, y + 1, r));
	assert(collides(board, SHAPE_O, x, y - 1, r));

	assert(!collides(board, SHAPE_O, x + 2, y, r));
	assert(!collides(board, SHAPE_O, x - 2, y, r));
	assert(!collides(board, SHAPE_O, x, y + 2, r));
	assert(!collides(board, SHAPE_O, x, y - 2, r));

	for (int i = 0; i < 5; ++i)
		board[i] = ~board[i];
	// print_board(board);
	assert(!collides(board, SHAPE_O, x, y, r));
	assert(collides(board, SHAPE_O, x + 1, y, r));
	assert(collides(board, SHAPE_O, x - 1, y, r));
	assert(collides(board, SHAPE_O, x, y + 1, r));
	assert(collides(board, SHAPE_O, x, y - 1, r));
}

static void test_board_clear(void)
{
	static uint64_t mask = (uint16_t)-1;
	int x = 3;
	int y1 = 14, y2 = 2;
	int r = 0;

	board_t board = {0};
	write(board, SHAPE_O, x, y1, r);
	write(board, SHAPE_O, x, y2, r);
	board[4] |= mask << 16 * 3;
	board[2] |= mask;
	// print_board(board);
	clear(board);
	// print_board(board);

	board_t board2 = {0};
	write(board2, SHAPE_O, x, y1 + 1, r);
	write(board2, SHAPE_O, x, y2 + 2, r);

	assert(0 == memcmp(board, board2, sizeof(board_t)));
}

static void test_board_bounds(void)
{
	board_t board = {0};
	assert(collides(board, SHAPE_I, 3, -1, 1)); // top
	assert(collides(board, SHAPE_I, -1, 12, 0)); // left
	assert(collides(board, SHAPE_I, 3, 17, 1)); // bottom
	assert(collides(board, SHAPE_I, 7, 12, 0)); // right

	assert(!collides(board, SHAPE_I, 3, -1, 0)); // top
	assert(!collides(board, SHAPE_I, -2, 12, 1)); // left
	assert(!collides(board, SHAPE_I, 3, 18, 0)); // bottom
	assert(!collides(board, SHAPE_I, 7, 12, 1)); // right
}

static void test_board(void)
{
	test_board_collision();
	test_board_clear();
	test_board_bounds();
}
#else
static void test_board(void) {}
#endif
