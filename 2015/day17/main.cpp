#include <iostream>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto data = utils::toIntV(File::readAllLines(argv[1]));

	{
		std::vector<std::vector<int>> combinations;

		utils::genCombinationSum<int>(data, utils::toInt(argv[2]), combinations);

		PRINTF(("PART_A: %zd", combinations.size()));

		{
			int min = std::numeric_limits<int>::max();
			int minCount = 0;

			for (auto &c : combinations) {
				if (c.size() < min) {
					min      = c.size();
					minCount = 1;

				} else if (c.size() == min) {
					minCount++;
				}
			}

			PRINTF(("PART_B: %d", minCount));
		}
	}
}
