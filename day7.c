#include <stdio.h>
#include <time.h>
#include "usfio.h"
#include "usfmath.h"

#define INPUTFILE "day7.txt"

int32_t main(void) {
	char **input;
	uint64_t nlines, linelen;
	input = usf_ftost(INPUTFILE, &nlines);
	linelen = strlen(input[0]);

	uint64_t descending[linelen + 2]; /* Padding to avoid bounds check */
	memset(descending, 0, sizeof(descending));
	descending[strchr(input[0], 'S') - input[0] + 1] = 1; /* Original beam */

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	uint64_t i, j, total, nsplits;
	char *c;
	for (total = 0, i = 1; i < nlines; i++) for (j = 1, c = input[i]; *c; j++, c++) {
		if (*c == '.') continue;
		if ((nsplits = descending[j])) {
			descending[j - 1] += nsplits;
			descending[j + 1] += nsplits;
			descending[j] = 0;
			total++; /* Part 1 */
		}
	}

	uint64_t total2;
	for (total2 = 0, i = 0; i < linelen + 2; i++)
		total2 += descending[i]; /* Part 2 */
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("The tachyon beam splits [%lu] times.\n", total);
	printf("The tachyon beam ends up in [%lu] timelines.\n", total2);

	printf("Took %f nanoseconds.\n", usf_elapsedtimens(start, end));
	usf_freetxt(input, 1);
}
