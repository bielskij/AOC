#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct SnailFishNumber {
	int value;
	int depth;

	SnailFishNumber() {
		this->value = 0;
		this->depth = 0;
	}

	SnailFishNumber(int value, int depth) {
		this->value = value;
		this->depth = depth;
	}
};


static std::vector<SnailFishNumber> sumAndReduceNumbers(std::vector<SnailFishNumber> a, std::vector<SnailFishNumber> b) {
	bool exploded;
	bool splitted;

	std::copy(b.begin(), b.end(), std::back_inserter(a));

	for (auto &number : a) {
		number.depth++;
	}

	// Reduce
	do {
		auto itLast = std::prev(a.end());

		exploded = false;
		splitted = false;

		// explode
		for (auto itCurr = a.begin(); itCurr != itLast; itCurr++) {
			auto itNext = std::next(itCurr);

			if (itCurr->depth > 4 && (itCurr->depth == itNext->depth)) {
				if (itCurr != a.begin()) {
					std::prev(itCurr)->value += itCurr->value;
				}

				if (itCurr != itLast) {
					std::next(itNext)->value += itNext->value;
				}

				itCurr = a.erase(itCurr);

				itCurr->depth--;
				itCurr->value = 0;

				exploded = true;
				break;
			}
		}

		// split
		if (! exploded) {
			for (auto itCurr = a.begin(); itCurr != a.end(); itCurr++) {
				if (itCurr->value > 9) {
					int newVal = itCurr->value / 2;

					itCurr->depth++;
					itCurr->value = newVal + (itCurr->value % 2);

					a.insert(itCurr, SnailFishNumber(newVal, itCurr->depth));

					splitted = true;
					break;
				}
			}
		}
	} while (exploded || splitted);

	return a;
}


static int calcMagnitude(std::vector<SnailFishNumber> snailfishNumber) {
	while (snailfishNumber.size() > 1) {
		for (auto itCurr = snailfishNumber.begin(); itCurr != std::prev(snailfishNumber.end()); itCurr++) {
			auto itNext = std::next(itCurr);

			if (itCurr->depth == itNext->depth) {
				itCurr->value = 3 * itCurr->value + 2 * itNext->value;
				itCurr->depth--;

				snailfishNumber.erase(itNext);
				break;
			}
		}
	}

	return snailfishNumber.begin()->value;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<std::vector<SnailFishNumber>> snailfishNumbers;

		for (const auto &l : lines) {
			std::vector<SnailFishNumber> number;

			int depth = 0;

			for (char c : l) {
				switch (c) {
					case '[':
						depth++;
						break;

					case ']':
						depth--;
						break;

					default:
						if (c != ',') {
							number.push_back(SnailFishNumber(c - '0', depth));
						}
						break;
				}
			}

			snailfishNumbers.push_back(number);
		}

//		for (const auto &n : snailfishNumbers) {
//			for (const auto &e : n) {
//				PRINTF(("%d, %d", e.value, e.depth));
//			}
//			PRINTF((" "));
//		}

		{
			std::vector<SnailFishNumber> sum = snailfishNumbers[0];

			for (int i = 1; i < snailfishNumbers.size(); i++) {
				sum = sumAndReduceNumbers(sum, snailfishNumbers[i]);
			}

			PRINTF(("PART_A: %d", calcMagnitude(sum)));
		}

		{
			int partB = 0;

			for (int i = 0; i < snailfishNumbers.size(); i++) {
				for (int j = i + 1; j < snailfishNumbers.size(); j++) {
					partB = std::max(partB, calcMagnitude(
						sumAndReduceNumbers(
							snailfishNumbers[i], snailfishNumbers[j]
						)
					));

					partB = std::max(partB, calcMagnitude(
						sumAndReduceNumbers(
							snailfishNumbers[j], snailfishNumbers[i]
						)
					));
				}
			}

			PRINTF(("PART_B: %d", partB));
		}
	}


	return 0;
}
