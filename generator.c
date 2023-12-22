#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "board.c"

void generate_shapes(char *filename)
{
	static char *u16 = "unsigned short";
	static char *u64 = "unsigned long long";

	FILE *fp = fopen(filename, "w");
	fprintf(fp, "%s shapes[N_SHAPES][4] = {\n", u64);
	// write each shape as a 64 bit int
	for (int shape = 0; shape < N_SHAPES; ++shape) {
		fprintf(fp, "\t[%s] = {\n", shape_type_to_string[shape]);
		for (int k = 0; k < 4; ++k) {
			fprintf(fp, "\t\t0b");
			for (int i = 0; i < 4; ++i) {
				fprintf(fp, "000000000");
				for (int j = 0; j < 4; ++j) {
					fprintf(fp, "%d", 0 != shapes[shape][k][3 - i][3 - j]);
				}
				fprintf(fp, "000");
			}
			fprintf(fp, ",\n");
		}
		fprintf(fp, "\t},\n");
	}
	fprintf(fp, "};");

	// also add some masks
	fprintf(fp, "\n\n %s bounds_mask = 0b", u64);
	for (int y = 0; y < 4; ++y)
		for (int x = 0; x < 16; ++x)
			fprintf(fp, "%1d", 15 - x >= WIDTH);
	fprintf(fp, ";");
	fprintf(fp, "\n %s row_mask = 0b0000001111111111;", u16);
	fprintf(fp, "\n %s col_mask = 0b", u64);
	for (int y = 0; y < 4; ++y)
		for (int x = 0; x < 16; ++x)
			fprintf(fp, "%1d", x == 15);
	fprintf(fp, ";");
	fclose(fp);
}

int main(void)
{
	generate_shapes("generated/bitshape.c");
	return 0;
}
