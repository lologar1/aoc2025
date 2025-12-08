#include <stdio.h>
#include <time.h>
#include "usfio.h"
#include "usfmath.h"

#define INPUTFILE "day6.txt"
#define NSPACES(STR) (strspn((STR), " "))

int32_t main(void) {
	/* COMMENTS: Day 6 is a bit uglier compared to the previous days because it's mostly string
	 * manipulation... I'm not very keen on revising it as it is lol */
	char **input, *c, in;
	uint64_t nlines, nnums, i, j;
	input = usf_ftost(INPUTFILE, &nlines);

	/* Count number of nums per line */
	for (nnums = in = 0, c = input[0]; *c; c++) {
		if (in && *c == ' ') in = 0;
		else if (!in && *c != ' ') { in = 1; nnums++; }
	}

	char ops[nnums];
	for (c = input[nlines - 1], i = 0; i < nnums; i++, c++)
		ops[i] = *(c += NSPACES(c));

	struct timespec start, end;
	double time = 0.0f;
	clock_gettime(CLOCK_MONOTONIC, &start);

	/* Part 1 */
	uint64_t numbers[nnums][nlines - 1];
	for (i = 0; i < nlines - 1; i++) for (c = input[i], j = 0; j < nnums; j++)
		numbers[j][i] = strtoul(c, &c, 10);

	uint64_t total, subtotal;
	for (c = input[i], total = j = 0; j < nnums; j++, c++) {
		if (ops[j] == '*') for (subtotal = 1, i = 0; i < nlines - 1; i++) subtotal *= numbers[j][i];
		else for (subtotal = i = 0; i < nlines - 1; i++) subtotal += numbers[j][i];
		total += subtotal;
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	time += usf_elapsedtimens(start, end);
	printf("Cephalopod's math homework first checksum is [%lu].\n", total);

	clock_gettime(CLOCK_MONOTONIC, &start);

	/* Part 2 */
	char numstring[256]; /* Safe size */
	uint64_t linelen, k, total2;
	linelen = strlen(input[i]); /* All lines same size, space-padded */

	subtotal = ops[0] == '*' ? 1 : 0;
	for (total2 = k = i = 0; i < linelen + 1; i++) {
		if (input[nlines - 1][i + 1] > ' ' || i == linelen) { /* Special case for last */
			total2 += subtotal;
			subtotal = ops[++k] == '*' ? 1 : 0;
			continue;
		}

		for (c = numstring, j = 0; j < nlines - 1; j++)
			if ((in = input[j][i]) != ' ') *c++ = in; /* Build */
		*c = '\0'; /* Terminate */

		if (ops[k] == '*') subtotal *= strtoul(numstring, NULL, 10);
		else subtotal += strtoul(numstring, NULL, 10);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("Cephalopod's math homework second checksum is [%lu].\n", total2);

	printf("Took %f nanoseconds.\n", time + usf_elapsedtimens(start, end));
	usf_freetxt(input, 1);
}
