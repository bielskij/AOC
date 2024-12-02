#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<int> left;
	std::vector<int> right;

	for (const auto &l : lines) {
		auto numStr = utils::strTok(l, ' ');

		left.push_back(utils::toInt(numStr[0]));
		right.push_back(utils::toInt(numStr[3]));
	}

	std::sort(left.begin(), left.end());
	std::sort(right.begin(), right.end());

	{
		int64_t result = 0;

		auto itLeft  = left.begin();
		auto itRight = right.begin();
	
		while (itLeft != left.end()) {
			result += std::abs(*itLeft++ - *itRight++);;
		}

		PRINTF(("A: %ld", result));
	}

	{
		std::map<int, int> tmp;

		for (auto &n : right) {
			tmp[n]++;
		}

		int64_t result = 0;

		for (auto &t : left) {
			result += (t * tmp[t]);
		}

		PRINTF(("B: %ld", result));
	}

	return 0;
}
