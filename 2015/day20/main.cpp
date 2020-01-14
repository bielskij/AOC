#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int partA(int n) {
	int sum = 0;
	int d = sqrt(n) + 1;

	for (int i = 1; i <= d; i++) {
		if (n % i == 0) {
			sum += i;
			sum += n / i;
		}
	}

	return sum * 10;
}


int partB(int n) {
	int sum = 0;
	int d = sqrt(n) + 1;

	for (int i = 1; i <= d; i++) {
		if (n % i == 0) {
			if (i <= 50) {
				sum += n/i;
			}

			if (n / i <= 50) {
				sum += i;
			}
		}
	}

	return sum * 11;
}


int main(int argc, char *argv[]) {
	{
		int n = 1;

		while (partA(n) < utils::toInt(argv[1])) {
			n += 1;
		}

		PRINTF(("PART_A: %d", n));
	}

	{
		int n = 1;

		while (partB(n) < utils::toInt(argv[1])) {
			n += 1;
		}

		PRINTF(("PART_B: %d", n));
	}
}
