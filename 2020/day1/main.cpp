#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


static int _solve(const std::vector<int> &samples, int num) {
	std::vector<std::vector<int>> combinations;

	utils::genCombinations(combinations, samples.size(), num);

	for (auto v : combinations) {
		int sum = 0;

		for (auto idx : v) {
			sum += samples[idx];
		}

		if (sum == 2020) {
			int mul = 1;

			for (auto idx : v) {
				mul *= samples[idx];
			}

			return mul;
		}
	}
}



int main(int argc, char *argv[]) {
	auto numbers = utils::toIntV(File::readAllLines(argv[1]));

	PRINTF(("PART_A: %d", _solve(numbers, 2)));
	PRINTF(("PART_B: %d", _solve(numbers, 3)));
}
