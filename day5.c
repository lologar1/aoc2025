#include <stdio.h>
#include <time.h>
#include "usfskiplist.h"
#include "usfio.h"
#include "usfmath.h"

#define INPUTFILE "day5.txt"

void findspot(usf_skiplist *ranges, usf_skipnode **prev, usf_skipnode **next, uint64_t base);

int32_t main(void) {
	char **input, *c;
	uint64_t nlines, i;
	input = usf_ftost(INPUTFILE, &nlines);

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	usf_skiplist *ranges;
	usf_skipnode *prev, *next, *node;
	uint64_t low, high;
	for (ranges = usf_newsk(), i = 0; input[i][0]; i++) { /* Double \n gives empty substring */
		low = strtoul(input[i], &c, 10);
		high = strtoul(++c, NULL, 10);

		findspot(ranges, &prev, &next, low);

		if (prev) {
			low = prev->data.u < low - 1 ? low : prev->index; /* Merge under */
			high = USF_MAX(prev->data.u, high); /* Merge within */
		}

		while (next && next->index <= high + 1) { /* Merge over */
			prev = next;
			next = next->nextnodes[0];
			high = USF_MAX(usf_skdel(ranges, prev->index).u, high);
		}

		usf_skset(ranges, low, USFDATAU(high));
	}

	uint64_t total, n;
	for (total = 0, i++; i < nlines; i++) { /* Part 1 */
		n = strtoul(input[i], NULL, 10);
		findspot(ranges, &prev, &next, n);
		if ((next && next->index == n) || (prev && prev->data.u >= n)) total++;
	}

	uint64_t total2;
	for (total2 = 0, node = ranges->base[0]; node; node = node->nextnodes[0]) /* Part 2 */
		total2 += node->data.u - node->index + 1;
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("There are [%lu] fresh available ingredient IDs.\n", total);
	printf("All ingredient ID ranges cover [%lu] different ingredient IDs.\n", total2);

	printf("Took %f nanoseconds.\n", usf_elapsedtimens(start, end));
	usf_freesk(ranges);
	usf_freetxt(input, 1);
}

void findspot(usf_skiplist *ranges, usf_skipnode **prev, usf_skipnode **next, uint64_t base) {
	/* Find prev and next range bases for a given range base */
	int32_t skiplevel;
	usf_skipnode **skipframe;
	skipframe = ranges->base; *prev = NULL;
	for (skiplevel = USF_SKIPLIST_FRAMESIZE - 1; skiplevel >= 0; skiplevel--) {
		while ((*next = skipframe[skiplevel])) {
			if ((*next)->index >= base) break; /* Overshot or match */
			skipframe = (*next)->nextnodes;
			*prev = *next;
		}
	}
}
