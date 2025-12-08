#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "usfio.h"
#include "usfhashmap.h"
#include "usfmath.h"

#define INPUTFILE "day2.txt"

uint64_t pow10i[20]; /* 10^19 is the highest power of 10 smaller than 2^64-1 */

uint64_t repeat(uint64_t section, uint64_t len, uint64_t repetitions);
uint64_t countdigits(uint64_t n);
int32_t clamp(uint64_t lownum, uint64_t highnum, uint64_t *lowrange, uint64_t *highrange, uint64_t repetitions);

int32_t main(void) {
	usf_hashmap *invalids, *invalids2;

	char **input;
	uint64_t nranges;
	input = usf_scsplit(usf_ftos(INPUTFILE, NULL), ',', &nranges);

	invalids = usf_newhm();
	invalids2 = usf_newhm();

	/* Generate powers of 10 */
	uint64_t p, i;
	for (p = 1, i = 0; i < 20; p *= 10, i++) pow10i[i] = p;

	char *lowstr, *highstr;
	uint64_t lowrange, highrange, lownum, highnum;

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	for (i = 0; i < nranges; i++) {
		lowstr = input[i];
		lownum = strtoul(lowstr, &highstr, 10);
		highnum = strtoul(highstr + 1, NULL, 10);

		for (p = 2; p < 21; p++) { /* Max pattern repeat: 20 times in uint64_t */
			if (!clamp(lownum, highnum, &lowrange, &highrange, p)) continue; /* Not within range */

			for (; lowrange <= highrange; lowrange++) {
				usf_inthmput(invalids2, repeat(lowrange, countdigits(lowrange), p), USFTRUE);

				/* For part 1 */
				if (p == 2) usf_inthmput(invalids, repeat(lowrange, countdigits(lowrange), p), USFTRUE);
			}
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	uint64_t total, total2;
	usf_data **arr;

	for (arr = invalids->array, total = i = 0; i < invalids->capacity; i++) {
		if (arr[i] == NULL || arr[i] == (usf_data *) invalids) continue;
		total += arr[i][0].u;
	}

	for (arr = invalids2->array, total2 = i = 0; i < invalids2->capacity; i++) {
		if (arr[i] == NULL || arr[i] == (usf_data *) invalids) continue;
		total2 += arr[i][0].u;
	}

	printf("Sum of invalid IDs (pattern repeat of 2) is [%lu].\n", total);
	printf("Sum of invalid IDs (any sized pattern repeat) is [%lu].\n", total2);

	printf("Took %f nanoseconds.\n", usf_elapsedtimens(start, end));
	usf_freehm(invalids); usf_freehm(invalids2);
	usf_freetxt(input, 1);
}

uint64_t repeat(uint64_t section, uint64_t len, uint64_t repetitions) {
	/* Repeat this number section times as if it were written in decimal */
	uint64_t sum, i;
	for (sum = i = 0; i < repetitions; i++)
		sum += section * pow10i[len * i];

	return sum;
}

uint64_t countdigits(uint64_t n) {
	uint64_t digits;
	for (digits = 0; n; digits++) n /= 10;

	return digits;
}

int32_t clamp(uint64_t lownum, uint64_t highnum, uint64_t *lowrange, uint64_t *highrange, uint64_t repetitions) {
	/* Clamp numbers specified by lowstr and highstr to their respective ceil or floor allowed
	 * patterns according to the number of repetitions requested */
	uint64_t lowdigits, highdigits;

	/* Get next (as we're nudging up) power of 10 which has enough digits */
	if ((lowdigits = countdigits(lownum)) % repetitions != 0)
		lownum = pow10i[(lowdigits += repetitions - lowdigits % repetitions) - 1];

	/* Get 1 less than previous power of 10 to have enough digits (nudging down) */
	if ((highdigits = countdigits(highnum)) % repetitions != 0)
		highnum = pow10i[highdigits -= highdigits % repetitions] - 1;

	if (highnum < lownum) return 0; /* Adjusted bounds fall outside range */

	/* Further clamp numbers to nearest valid pattern (either up or down depending) and return the pattern
	 * section in lowrange and highrange */
	uint64_t shift, sectionwidth, section;

	sectionwidth = lowdigits / repetitions; shift = pow10i[sectionwidth * (repetitions - 1)];
	section = lownum / shift;
	*lowrange = (lownum <= repeat(section, sectionwidth, repetitions) ? section : section + 1);

	sectionwidth = highdigits / repetitions; shift = pow10i[sectionwidth * (repetitions - 1)];
	section = highnum / shift;
	*highrange = (highnum >= repeat(section, sectionwidth, repetitions) ? section : section - 1);

	if (*highrange < *lowrange) return 0; /* Adjusted bounds fall outside range */

	return 1;
}
