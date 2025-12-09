#include <stdio.h>
#include <time.h>
#include "usfio.h"
#include "usfmath.h"

#define INPUTFILE "day8.txt"
#define NITERATIONS 1000

typedef struct {
	double dist;
	uint64_t a;
	uint64_t b;
} Distance;

int32_t cmp(const void *a, const void *b) { return ((Distance *) a)->dist > ((Distance *) b)->dist ? 1 : -1; }

int32_t main(void) {
	char **input, *c;
	uint64_t nlines, npairs;
	input = usf_ftost(INPUTFILE, &nlines);

	uint64_t points[nlines][3], i, j, n;
	for (i = 0; i < nlines; i++) {
		points[i][0] = strtoul(c = input[i], &c, 10);
		points[i][1] = strtoul(++c, &c, 10);
		points[i][2] = strtoul(++c, NULL, 10);
	}

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	npairs = (nlines - 1) * nlines >> 1;
	Distance *distances = malloc(npairs * sizeof(Distance));
	for (n = i = 0; i < nlines; i++) for (j = i + 1; j < nlines; j++, n++) {
		distances[n].dist = sqrt(
				(points[i][0] - points[j][0]) * (points[i][0] - points[j][0])
				+ (points[i][1] - points[j][1]) * (points[i][1] - points[j][1])
				+ (points[i][2] - points[j][2]) * (points[i][2] - points[j][2]));
		distances[n].a = i; distances[n].b = j;
	}
	qsort(distances, npairs, sizeof(Distance), cmp);

	uint64_t groups[nlines], members[nlines];
	for (n = 0; n < nlines; n++) {
		groups[n] = 1;
		members[n] = nlines;
	}

	uint64_t ngroups, seen, a, b, g;
	for (ngroups = seen = n = 0; seen < nlines || ngroups != 1; n++) {
		a = distances[n].a; b = distances[n].b;
		i = members[a]; j = members[b];

		if (i == nlines && j == nlines) { /* New group */
			members[a] = members[b] = a;
			if (n < NITERATIONS) { groups[a] = 2; groups[b] = 0; } /* First merge */
			ngroups++;
			seen += 2;
			continue;
		}

		if (i == nlines || j == nlines) { /* Link */
			if (i == nlines) { USF_SWAP(i, j); USF_SWAP(a, b); }
			members[b] = members[a];
			if (n < NITERATIONS) { groups[i]++; groups[j] = 0; }
			seen++;
			continue;
		}

		if (i == j) continue; /* Same group */
		if (n < NITERATIONS) { groups[i] += groups[j]; groups[j] = 0; }
		for (g = 0; g < nlines; g++) if (members[g] == j) members[g] = i; /* Switch over */
		ngroups--;
	}
	qsort(groups, nlines, sizeof(uint64_t), usf_indu64cmp);

	uint64_t total, total2;
	total = groups[nlines - 1] * groups[nlines - 2] * groups[nlines - 3];
	total2 = points[a][0] * points[b][0];
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("The product of the sizes of the three largest circuits is [%lu].\n", total);
	printf("The product of the X coordinates of the last junction to complete circuit is [%lu].\n", total2);

	printf("Took %f nanoseconds.\n", usf_elapsedtimens(start, end));
	free(distances);
	usf_freetxt(input, 1);
}
