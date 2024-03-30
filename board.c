#define BOARD

enum shape_type {
	SHAPE_I,
	SHAPE_O,
	SHAPE_S,
	SHAPE_Z,
	SHAPE_J,
	SHAPE_L,
	SHAPE_T,
	N_SHAPES,
};

char *shape_type_to_string[N_SHAPES] = {
	[SHAPE_I] = "SHAPE_I",
	[SHAPE_O] = "SHAPE_O",
	[SHAPE_S] = "SHAPE_S",
	[SHAPE_Z] = "SHAPE_Z",
	[SHAPE_J] = "SHAPE_J",
	[SHAPE_L] = "SHAPE_L",
	[SHAPE_T] = "SHAPE_T",
};

int shapes[N_SHAPES][4][4][4] =
{
	[SHAPE_I] = {
		{
			{ 0, 0, 0, 0, },
			{ 1, 1, 1, 1, },
			{ 0, 0, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 1, 0, },
			{ 0, 0, 1, 0, },
			{ 0, 0, 1, 0, },
			{ 0, 0, 1, 0, },
		},
		{
			{ 0, 0, 0, 0, },
			{ 1, 1, 1, 1, },
			{ 0, 0, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 1, 0, },
			{ 0, 0, 1, 0, },
			{ 0, 0, 1, 0, },
			{ 0, 0, 1, 0, },
		},
	},
	[SHAPE_O] = {
		{
			{ 0, 0, 0, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 0, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 0, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 0, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 2, 2, 0, },
			{ 0, 0, 0, 0, },
		},
	},
	[SHAPE_S] = {
		{
			{ 0, 0, 0, 0, },
			{ 0, 3, 3, 0, },
			{ 3, 3, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 3, 0, 0, },
			{ 0, 3, 3, 0, },
			{ 0, 0, 3, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 0, 0, },
			{ 0, 3, 3, 0, },
			{ 3, 3, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 3, 0, 0, },
			{ 0, 3, 3, 0, },
			{ 0, 0, 3, 0, },
			{ 0, 0, 0, 0, },
		},
	},
	[SHAPE_Z] = {
		{
			{ 0, 0, 0, 0, },
			{ 4, 4, 0, 0, },
			{ 0, 4, 4, 0, },
			{ 0, 0, 0, 0, },
		},
			{
			{ 0, 0, 4, 0, },
			{ 0, 4, 4, 0, },
			{ 0, 4, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 0, 0, },
			{ 4, 4, 0, 0, },
			{ 0, 4, 4, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 4, 0, },
			{ 0, 4, 4, 0, },
			{ 0, 4, 0, 0, },
			{ 0, 0, 0, 0, },
		},
	},
	[SHAPE_J] = {
		{
			{ 0, 0, 0, 0, },
			{ 5, 5, 5, 0, },
			{ 0, 0, 5, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 5, 5, 0, },
			{ 0, 5, 0, 0, },
			{ 0, 5, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 5, 0, 0, 0, },
			{ 5, 5, 5, 0, },
			{ 0, 0, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 5, 0, 0, },
			{ 0, 5, 0, 0, },
			{ 5, 5, 0, 0, },
			{ 0, 0, 0, 0, },
		},
	},
	[SHAPE_L] = {
		{
			{ 0, 0, 0, 0, },
			{ 6, 6, 6, 0, },
			{ 6, 0, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 6, 0, 0, },
			{ 0, 6, 0, 0, },
			{ 0, 6, 6, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 0, 6, 0, },
			{ 6, 6, 6, 0, },
			{ 0, 0, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 6, 6, 0, 0, },
			{ 0, 6, 0, 0, },
			{ 0, 6, 0, 0, },
			{ 0, 0, 0, 0, },
		},
	},
	[SHAPE_T] = {
		{
			{ 0, 0, 0, 0, },
			{ 7, 7, 7, 0, },
			{ 0, 7, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 7, 0, 0, },
			{ 0, 7, 7, 0, },
			{ 0, 7, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 7, 0, 0, },
			{ 7, 7, 7, 0, },
			{ 0, 0, 0, 0, },
			{ 0, 0, 0, 0, },
		},
		{
			{ 0, 7, 0, 0, },
			{ 7, 7, 0, 0, },
			{ 0, 7, 0, 0, },
			{ 0, 0, 0, 0, },
		},
	},
};

#define HEIGHT 22
#define WIDTH 10

typedef int board_t[HEIGHT][WIDTH];

static int shape_at(int shape[4][4], int i, int j)
{
	return shape[i][j];
}

static int board_at(board_t board, int i, int j)
{
	return board[i][j];
}

static void print_board(board_t board)
{
	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int j = 0; j < WIDTH; ++j)
			putchar(board[i][j] ? board[i][j] + '0' : '.');
		putchar('\n');
	}
}

static int board_height(board_t board)
{
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			if (board[i][j])
				return HEIGHT - i;
	return 0;
}

static float board_variance(board_t board)
{
	int sum = 0;
	int heights[WIDTH] = {0};
	for (int j = 0; j < WIDTH; ++j)
	{
		for (int i = 0; i < HEIGHT; ++i)
			if (board[i][j])
			{
				sum += heights[j] = HEIGHT - i;
				break;
			}
	}
	float avg = (float)sum / 10;
	float variance = 0;
	for (int j = 0; j < WIDTH; ++j)
	{
		float diff = heights[j] - avg;
		variance += diff > 0 ? diff : -diff;
	}
	return variance;
}

static int board_holes(board_t board)
{
	int holes = 0;
	for (int j = 0; j < WIDTH; ++j)
		for (int i = 0, flag = 0; i < HEIGHT; ++i)
			if (board[i][j])
				flag = 1;
			else
				holes += flag;
	return holes;
}

static int board_row_transitions(board_t board)
{
	int result = 0;
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH - 1; ++j)
			result += board[i][j] && !board[i][j + 1];
	return result;
}

static int board_col_transitions(board_t board)
{
	int result = 0;
	for (int i = 0; i < HEIGHT - 1; ++i)
		for (int j = 0; j < WIDTH; ++j)
			result += board[i][j] & !board[i + 1][j];
	return result;
}

static bool board_is_empty(board_t board)
{
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			if (board[i][j])
				return false;
	return true;
}

static int board_well_depth(board_t board, int j)
{
	int count = 0;
	for (int i = 0; i < HEIGHT; ++i) {
		if ((j > 0 && board[i][j - 1] != 0) &&
		    (j < WIDTH - 1 && board[i][j + 1] != 0)) {
			if (board[i][j] != 0)
				break;
			else
				++count;
		}
		else
			count = 0;
	}
	return count;
}

static int board_wells(board_t board)
{
	int max_depth = 0;
	int total_depth = 0;
	for (int j = 0; j < WIDTH; ++j) {
		int depth = board_well_depth(board, j);
		if (depth > max_depth)
			max_depth = depth;
		total_depth += depth;
	}
	return total_depth - max_depth;
}

static int board_cell_count(board_t board)
{
	int result = 0;
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			result += board[i][j] != 0;
	return result;
}

static bool collides(board_t board, int shape, int x, int y, int r)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			if (!shapes[shape][r][i][j])
				continue;
			int iy = i + y;
			int jx = j + x;
			if (iy < 0 || iy >= HEIGHT || jx < 0 || jx >= WIDTH || board[iy][jx])
				return true;
		}
	return false;
}

static bool row(board_t board, int i)
{
	for (int j = 0; j < WIDTH; ++j)
		if (!board[i][j])
			return false;
	return true;
}

static int clear(board_t board)
{
	bool clears[HEIGHT];
	int l = 0;
	for (int i = 0; i < HEIGHT; ++i)
		l += clears[i] = row(board, i);

	for (int i = HEIGHT - 1, k = 0; i >= 0; --i)
	{
		while (clears[i - k])
			++k;
		for (int j = 0; j < WIDTH; ++j)
			board[i][j] = i - k < 0 ? 0 : board[i - k][j];
	}

	return l;
}

static int write(board_t board, int shape, int x, int y, int r)
{
	assert(!collides(board, shape, x, y, r));
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			int cell = shapes[shape][r][i][j];
			if (cell)
				board[y + i][x + j] = cell;
		}
	return clear(board);
}
