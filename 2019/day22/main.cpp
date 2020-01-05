#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <unistd.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


enum ShufflingTechnique {
	CUT,
	DEAL_NEW_STACK,
	DEAL_INCREMENT
};


static int64_t getPosition(std::vector<std::pair<ShufflingTechnique, int64_t>> &techniques, int64_t max, int64_t value) {
	for (auto &t : techniques) {
		switch (t.first) {
			case ShufflingTechnique::CUT:
				{
					value = (max - t.second + value) % max;
				}
				break;

			case ShufflingTechnique::DEAL_INCREMENT:
				{
					value = (value * t.second) % max;
				}
				break;

			case ShufflingTechnique::DEAL_NEW_STACK:
				{
					value = max - value - 1;
				}
				break;
		}
	}

	return value;
}


int main(int argc, char *argv[]) {
	auto rules = File::readAllLines(argv[1]);

	{
		std::vector<std::pair<ShufflingTechnique, int64_t>> techniques;

		for (int i = 0; i < rules.size(); i++) {
			int value;

			if (sscanf(rules[i].c_str(), "cut %d", &value) == 1) {
				techniques.push_back(std::pair<ShufflingTechnique, int64_t>(ShufflingTechnique::CUT, value));

			} else if (sscanf(rules[i].c_str(), "deal with increment %d", &value) == 1) {
				techniques.push_back(std::pair<ShufflingTechnique, int64_t>(ShufflingTechnique::DEAL_INCREMENT, value));

			} else if (rules[i].compare("deal into new stack") == 0) {
				techniques.push_back(std::pair<ShufflingTechnique, int64_t>(ShufflingTechnique::DEAL_NEW_STACK, 0));
			}
		}

		{
			PRINTF(("PART_A: %" PRId64, getPosition(techniques, 10007, 2019)));
		}
	}
}
