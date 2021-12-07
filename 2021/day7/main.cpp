#include <functional>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int solve(const std::vector<int> &crabs, std::function<int(int, int)> func) {
	int ret = std::numeric_limits<int>::max();

	{
		std::set<int> sortedPoints(crabs.begin(), crabs.end());

		for (int i = *sortedPoints.begin(); i <= *sortedPoints.rbegin(); i++) {
			int sum = 0;

			for (auto p2 : crabs) {
				sum += func(i, p2);
			}

			if (sum < ret) {
				ret = sum;
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto points = utils::toIntV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	PRINTF(("PART_A: %d", solve(points, [](int a, int b) {
		return std::abs(a - b);
	})));

	PRINTF(("PART_B: %d", solve(points, [](int a, int b) {
		int diff = std::abs(a - b);

		return ((diff * (diff + 1)) / 2);
	})));

	return 0;
}
