#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"



int main(int argc, char *argv[]) {
	auto numbers = utils::toIntV(File::readAllLines(argv[1]));

	std::sort(numbers.begin(), numbers.end());

	numbers.insert(numbers.begin(), 0);
	numbers.push_back(*std::max_element(numbers.begin(), numbers.end()) + 3);

	{
		std::map<int, int> diff;

		int currentAdapter = 0;
		for (auto adapter : numbers) {
			diff[adapter - currentAdapter]++;

			currentAdapter = adapter;
		}

		PRINTF(("PART_A: %d", diff[1] * diff[3]));
	}

	{
		long long int val[numbers.size()] = { 0 };

		val[0] = 1;

		for (int i = 1; i < numbers.size(); i++) {
			long long int count = 0;

			for (int j = 1; j <= std::min(i, 3); j++) {
				if (numbers[i] - numbers[i - j] <= 3) {
					count += val[i - j];
				}
			}

			val[i] = count;
		}


		PRINTF(("PART_B: %lld", val[numbers.size() - 1]));
	}
}
