#include <stdio.h>
#include <time.h>
#include "usfio.h"
#include "usfmath.h"

#define INPUTFILE "day3.txt"

uint64_t findbest(char *bank, uint64_t offset, uint64_t nbatteries, uint64_t leftover);

int32_t main(void) {
	char **input;
	uint64_t nbanks, nbatteries;
	input = usf_ftost(INPUTFILE, &nbanks);
	nbatteries = strlen(input[0]); /* All banks have same size */

	uint64_t total, total2, i, j, subtotal, offset;
	char *bank;

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	for (total = total2 = i = 0; i < nbanks; i++) {
		/* Part 1 */
		bank = input[i];
		for (offset = subtotal = j = 0; j < 2; j++) {
			offset = findbest(bank, offset, nbatteries, 1 - j);
			subtotal *= 10; subtotal += bank[offset++] - 48;
		}
		total += subtotal;

		/* Part 2 */
		for (offset = subtotal = j = 0; j < 12; j++) {
			offset = findbest(bank, offset, nbatteries, 11 - j);
			subtotal *= 10; subtotal += bank[offset++] - 48;
		}
		total2 += subtotal;
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("Maximum joltage by activating two consecutive batteries is [%lu].\n", total);
	printf("Maximum joltage by activating twelve consecutive batteries is [%lu].\n", total2);

	printf("Took %f nanoseconds.\n", usf_elapsedtimens(start, end));
	usf_freetxt(input, 1);
}

uint64_t findbest(char *bank, uint64_t offset, uint64_t nbatteries, uint64_t leftover) {
	/* Find best char from '9' to '1' while leaving leftover chars at the end */
	char *candidate, nextbest;
	for (candidate = NULL, nextbest = '9'; candidate == NULL; nextbest--) {
		candidate = strchr(bank + offset, nextbest);
		if (nbatteries - (candidate - bank + 1) < leftover) candidate = NULL; /* Not enough batteries left */
	}

	return candidate - bank;
}
