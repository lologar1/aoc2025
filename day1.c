#include <stdio.h>
#include "usfio.h"
#include "usfmath.h"
#include <time.h>

#define INPUTFILE "day1.txt"

int32_t main(void) {
	char **input;
	uint64_t ninstructions, i, pointzero, passzero;

	input = usf_ftost(INPUTFILE, &ninstructions);

	int64_t turn, dial = 50;
	uint64_t fullturns, leftover;

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	for (pointzero = passzero = i = 0; i < ninstructions; i++) {
		turn = (input[i][0] == 'R' ? 1 : -1) * strtol(&input[i][1], NULL, 10);
		fullturns = labs(turn) / 100;
		leftover = turn % 100;

		passzero += fullturns; /* Pass zero once per full turn */

		/* Starting at zero, leftover cannot cross zero */
		if (dial && (dial + leftover <= 0 || dial + leftover >= 100))
			passzero++;

		dial += leftover;
		if ((dial %= 100) == 0) pointzero++;
		if (dial < 0) dial += 100; /* Reset to positive offset */
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("Dial pointed zero after turn [%lu] times.\n", pointzero);
	printf("Dial passed zero [%lu] times.\n", passzero);

	printf("Took %f nanoseconds.\n", usf_elapsedtimens(start, end));
	usf_freetxt(input, 1);
}
