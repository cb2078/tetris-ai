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

// bit order:
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
// 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
// 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
// 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63

#include "generated\bitshape.c"

#define HEIGHT 20
#define WIDTH 10

typedef unsigned long long uint64_t;
typedef unsigned short uint16_t;

typedef uint64_t shape_t;
typedef uint64_t board_t[5];

static void shape_split(uint64_t shape, int y, uint64_t *top, uint64_t *bottom)
{
	*top = shape << y % 4 * 16;
	// left shifting by 64 is undefined in c
	*bottom = y % 4 ?
		shape >> (4 - y % 4) * 16 :
		0;
}

static shape_t shape_get(int shape_type, int x, int r)
{
	return x > 3 ?
		shapes[shape_type][r] << x - 3 :
		shapes[shape_type][r] >> 3 - x;
}

// NOTE this may need an offset parameter if the shape has been bit shifted
static int shape_at(shape_t shape, int i, int j)
{
	return 1 & shape >> j * 16 >> i + 3;
}

static int board_at(board_t board, int i, int j)
{
	return 1 & ((uint16_t *)board)[i] >> j;
}

static void print_board(board_t board)
{
	for (int i = 0; i < 20; ++i) {
		for (int j = 0; j < WIDTH; ++j)
			putchar(".#"[1 & ((uint16_t *)board)[i] >> j]);
		putchar('\n');
	}
	putchar('\n');
}

static int board_height(board_t board)
{
	for (int i = 0; i < HEIGHT; ++i)
		if (((uint16_t *)board)[i] & row_mask)
			return HEIGHT - i;
	return 0;
}

// TODO intrinsic
static int hamming_weight(uint64_t x)
{
	int result;
	for (result = 0; x; ++result)
		x &= x - 1;
	return result;
}
#define bit_count hamming_weight

static int board_holes(board_t board)
{
	uint16_t *board16 = (uint16_t *)board;
	int holes = 0;
	uint16_t flag = 0;
	for (int i = 0; i < 20; ++i) {
		flag |= board16[i];
		holes += bit_count(flag & ~board16[i]);
	}
	return holes;
}

static float board_variance(board_t board)
{
	int sum = 0;
	int heights[WIDTH] = {0};
	for (int j = 0; j < WIDTH; ++j)
		for (int i = 0; i < 5; ++i) {
			uint64_t result =  board[i] >> j & col_mask;
			if (!result)
				continue;
			heights[j] += i * 4;
			for (; !(result & 1); result >>= 16)
				++heights[j];
			sum += heights[j];
			break;
		}
	float avg = (float)sum / 10;
	float var = 0;
	for (int j = 0; j < WIDTH; ++j)
		var += heights[j] > avg ? heights[j] - avg : avg - heights[j];
	return var;
}

static bool collides(board_t board, int shape_type, int x, int y, int r)
{
	shape_t shape = shape_get(shape_type, x, r);
	uint64_t top, bottom; shape_split(shape, y, &top, &bottom);
	int i = (y&~3) / 4;
	return (board[i] | bounds_mask) & top || i < 0 && top ||
		i < 4 && (board[i + 1] | bounds_mask) & bottom || i >= 4 && bottom ||
		bit_count(top) + bit_count(bottom) < 4;
}

static int clear(board_t board)
{
	uint16_t *board16 = (uint16_t *)board;
	bool clears[HEIGHT];
	int l = 0;
	for (int i = 0; i < HEIGHT; ++i)
		l += clears[i] = (board16[i] & row_mask) == row_mask;

	for (int i = HEIGHT - 1, k = 0; i >= 0; --i) {
		while (clears[i - k])
			++k;
		board16[i] = i - k < 0 ? 0 : board16[i - k];
	}

	return l;
}

static int write(board_t board, int shape_type, int x, int y, int r)
{
	assert(!collides(board, shape_type, x, y, r));
	shape_t shape = shape_get(shape_type, x, r);
	int i = (y&~3) / 4;
	uint64_t top, bottom; shape_split(shape, y, &top, &bottom);
	if (i < 4)
		board[i + 1] |= bottom;
	if (i >= 0)
		board[i] |= top;
	return clear(board);
}
