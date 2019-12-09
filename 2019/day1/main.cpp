#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


static int _calculateFuelStepA(int mass) {
	int ret = (mass / 3) - 2;

	return ret < 0 ? 0 : ret;
}


static int _calculateFuelStepB(int mass) {
	int ret = 0;

	int cmass = mass;

	while (cmass > 0) {
		cmass = _calculateFuelStepA(cmass);

		ret += cmass;
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto samples = utils::toIntV(File::readAllLines(argv[1]));

	{
		int total = 0;

		for (auto it = samples.begin(); it != samples.end(); it++) {
			total += _calculateFuelStepA(*it);
		}

		PRINTF(("PART A: %d", total));
	}

	{
		int total = 0;

		for (auto it = samples.begin(); it != samples.end(); it++) {
			total += _calculateFuelStepB(*it);
		}

		PRINTF(("PART B: %d", total));
	}
}
