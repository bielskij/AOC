#include <list>
#include <cassert>
#include <deque>
#include <iterator>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int64_t solve(const std::vector<int64_t *> &numbers, int64_t multiplier, int turns) {
	std::list<int64_t *> decrypted(numbers.begin(), numbers.end());

	int64_t *zeroElement = nullptr;

	for (auto n : decrypted) {
		if (zeroElement == nullptr) {
			if (*n == 0) {
				zeroElement = n;
				break;
			}
		}
	}

	if (multiplier > 1) {
		for (auto n : decrypted) {
			*n *= multiplier;
		}
	}

	for (int turn = 0; turn < turns; turn++) {
		for (auto n : numbers) {
			auto srcIt  = std::find(decrypted.begin(), decrypted.end(), n);
			int  srcIdx = std::distance(decrypted.begin(), srcIt);
			int  dstIdx = utils::floorMod<int64_t>(*n + srcIdx, numbers.size() - 1);

			decrypted.erase(srcIt);

			{
				auto it = decrypted.begin();
				std::advance(it, dstIdx);
				decrypted.insert(it, n);
			}
		}
	}

	{
		int zeroIdx = std::distance(decrypted.begin(), std::find(decrypted.begin(), decrypted.end(), zeroElement));

		int64_t result = 0;

		for (int i = 1; i <= 3; i++) {
			auto it = decrypted.begin();

			std::advance(it, (zeroIdx + 1000 * i) % decrypted.size());

			result += *(*it);
		}

		return result;
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<int64_t *> numbers;

	for (auto &l : lines) {
		numbers.push_back(new int64_t(utils::toInt(l)));
	}

	PRINTF(("PART_A: %lld", solve(numbers,         1,  1)));
	PRINTF(("PART_B: %lld", solve(numbers, 811589153, 10)));

	return 0;
}
