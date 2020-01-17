#include <cinttypes>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


uint64_t solve(std::vector<int> &values, int groups) {
	uint64_t ret = std::numeric_limits<uint64_t>::max();

	int sum = 0;
	int groupSum = 0;

	for (auto &n : values) {
		sum += n;
	}

	groupSum = sum / groups;

	std::vector<std::vector<int>> out;

	utils::genCombinationSum<int>(values, groupSum, out);

	for (auto &s : out) {
		uint64_t sum = 1;

		for (auto &v : s) {
			sum *= v;
		}

		if (sum < ret) {
			ret = sum;
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto data = utils::toIntV(File::readAllLines(argv[1]));

	PRINTF(("PART_A: %" PRIu64, solve(data, 3)));
	PRINTF(("PART_B: %" PRIu64, solve(data, 4)));
}
