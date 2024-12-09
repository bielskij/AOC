#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static bool _check(int64_t expected, int64_t current, const std::vector<int64_t> &factors, int depth, bool isSum) {
	if (isSum) {
		current += factors[depth];

	} else {
		current *= factors[depth];
	}

	if (depth == factors.size() - 1) {
		return (current == expected);
	}

	if (current <= expected) {
		return _check(expected, current, factors, depth + 1, true) || _check(expected, current, factors, depth + 1, false);
	}

	return false;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int64_t partA = 0;

	for (const auto &l : lines) {
		auto nums = utils::toInt64tV(utils::strTok(l, ' '));

		bool found = _check(nums[0], nums[1], nums, 2, true) || _check(nums[0], nums[1], nums, 2, false);

		if (found) {
			partA += nums[0];
		}
	}

	PRINTF(("PART_A: %ld", partA));

	return 0;
}
