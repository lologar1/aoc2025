#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "usfio.h"
#include "usfmath.h"

#define INPUTFILE "day4.txt"

uint32_t cullrolls(uint32_t width, uint32_t height, char from[height][width], char to[height][width]);

int32_t main(void) {
	char **input;
	uint64_t nlines, ntiles, i;
	input = usf_ftost(INPUTFILE, &nlines);
	ntiles = strlen(input[0]); /* All lines same width; skip \0 */
	ntiles += 2; nlines += 2; /* Account for padding */

	/* Operating buffers */
	char from[nlines][ntiles], to[nlines][ntiles], (*temp)[ntiles], (*fromptr)[ntiles], (*toptr)[ntiles];
	for (i = 1; i < nlines - 1; i++) memcpy(from[i] + 1, input[i - 1], ntiles - 2); /* Copy only file data */
	fromptr = from; toptr = to; /* Set pointers to starting buffers */
	usf_freetxt(input, 1);

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	uint64_t total, total2, subtotal;
	total = total2 = 0;
	while ((subtotal = cullrolls(ntiles, nlines, fromptr, toptr))) {
		if (total == 0) total = subtotal; /* Part 1 */
		total2 += subtotal; /* Part 2 */

		temp = fromptr; fromptr = toptr; toptr = temp; /* Swap */
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("There are [%lu] rolls with less than 4 neighbors.\n", total);
	printf("It is possible to remove [%lu] rolls in total.\n", total2);

	printf("Took %f nanoseconds.\n", usf_elapsedtimens(start, end));
}

uint32_t cullrolls(uint32_t width, uint32_t height, char from[height][width], char to[height][width]) {
	/* Cull all accessible rolls (< 4 neighbors) from board [from] and write to new board [to] */
	static const int32_t dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	static const int32_t dy[8] = {1, 1, 1, 0, 0, -1, -1, -1};
	memset(to, 0, width * height * sizeof(char));

	/* Find which positions need to be culled */
	uint32_t x, y, adj;
	for (y = 1; y < height - 1; y++) for (x = 1; x < width - 1; x++) {
		if (from[y][x] == '.') {
			to[y][x] = 4; /* Not to be considered */
			continue;
		}

		for (adj = 0; adj < 8; adj++) /* No bounds checking since padding */
			to[y + dy[adj]][x + dx[adj]]++;
	}

	/* Copy culled array */
	uint32_t culled;
	for (culled = 0, y = 1; y < height - 1; y++) for (x = 1; x < width - 1; x++)
		if (to[y][x] < 4) { to[y][x] = '.'; culled++; }
		else to[y][x] = from[y][x];

	return culled;
}
