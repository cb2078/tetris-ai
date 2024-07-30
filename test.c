#if 1
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

	// TODO for the regular board
#if 0
	for (int i = 0; i < 5; ++i)
		board[i] = ~board[i];
	// print_board(board);
	assert(!collides(board, SHAPE_O, x, y, r));
	assert(collides(board, SHAPE_O, x + 1, y, r));
	assert(collides(board, SHAPE_O, x - 1, y, r));
	assert(collides(board, SHAPE_O, x, y + 1, r));
	assert(collides(board, SHAPE_O, x, y - 1, r));
#endif
}

static void test_board_clear(void)
{
#if 0
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
#endif
}

static void test_board_bounds(void)
{
	board_t board = {0};
	assert(collides(board, SHAPE_I, 3, -1, 1)); // top
	assert(collides(board, SHAPE_I, -1, 12, 0)); // left
	assert(collides(board, SHAPE_I, 3, HEIGHT - 3, 1)); // bottom
	assert(collides(board, SHAPE_I, 7, 12, 0)); // right

	assert(!collides(board, SHAPE_I, 3, -1, 0)); // top
	assert(!collides(board, SHAPE_I, -2, 12, 1)); // left
	assert(!collides(board, SHAPE_I, 3, HEIGHT - 2, 0)); // bottom
	assert(!collides(board, SHAPE_I, 7, 12, 1)); // right
}

static void test_board_features(void)
{
	board_t board;
	char *s =
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"     #    "
		"    ##    "
		"    ##    "
		"    ##    "
		"#/  ##    "
		"#/ ###    "
		"#//### ## "
		"###### ## "
		"###### ## ";
	read_board(board, (char (*)[WIDTH])s);
	assert(board_wells(board) == 7);
	assert(board_col_holes(board) == 0);
	assert(board_col_transitions(board) == 0);
	assert(board_row_transitions(board) == 36);

	s =
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"          "
		"    //    "
		"    #//   "
		" #  ##//  "
		" # /###/# "
		"##/ ##//##"
		"### ## /##"
		"#######/# "
		"// #######"
		"// /#//###"
		"## ##///##";
	read_board(board, (char (*)[WIDTH])s);
	assert(board_wells(board) == 4);
	assert(board_col_holes(board) == 4);
	assert(board_col_transitions(board) == 7);
	assert(board_row_transitions(board) == 34);
}

static void test_board(void)
{
	test_board_collision();
	test_board_clear();
	test_board_bounds();
	test_board_features();
}
#else
static void test_board(void) {}
#endif
