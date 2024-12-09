#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


enum class Operator {
	ADD,
	MUL,
	CON
};

static bool _check(int64_t expected, int64_t current, const std::vector<int64_t> &factors, int depth, const std::vector<Operator> &ops) {
	bool ret = false;

	for (const auto op : ops) {
		int64_t next = current;

		switch (op) {
			case Operator::ADD: 
				next += factors[depth]; 
				break;

			case Operator::MUL: 
				next *= factors[depth]; 
				break;

			case Operator::CON:
				{
					int mod = 1;
					auto tmp = factors[depth];

					while (tmp) {
						tmp /= 10;
						mod *= 10;
					}

					next *= mod;
					next += factors[depth];
				}
				break;
		}

		if (depth == factors.size() - 1) {
			ret = ret || (next == expected);

		} else {
			if (next <= expected) {
				ret = ret || _check(expected, next, factors, depth + 1, ops);
			}
		}

		if (ret) {
			break;
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int64_t partA = 0;
	int64_t partB = 0;

	for (const auto &l : lines) {
		auto nums = utils::toInt64tV(utils::strTok(l, ' '));

		if (_check(nums[0], nums[1], nums, 2, { Operator::ADD, Operator::MUL })) {
			partA += nums[0];
		}

		if (_check(nums[0], nums[1], nums, 2, { Operator::ADD, Operator::MUL, Operator::CON })) {
			partB += nums[0];
		}
	}

	PRINTF(("PART_A: %ld", partA));
	PRINTF(("PART_B: %ld", partB));

	return 0;
}
