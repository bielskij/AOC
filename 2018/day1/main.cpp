#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto data = utils::toInt64tV(File::readAllLines(argv[1]));

	{
		long long int frequency = 0;

		for (auto v : data) {
			frequency += v;
		}

		PRINTF(("PART_A: %lld", frequency));
	}

	{
		std::set<int> uniqueFrequencies;

		long long int frequency = 0;

		auto it = data.begin();
		while (true) {
			if (! uniqueFrequencies.insert(frequency).second) {
				PRINTF(("PART_B: %lld", frequency));
				break;
			}

			frequency += *it++;

			if (it == data.end()) {
				it = data.begin();
			}
		}
	}
}
