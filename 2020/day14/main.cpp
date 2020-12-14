#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<uint64_t, uint64_t> memoryA;
	std::map<uint64_t, uint64_t> memoryB;

	std::string mask;

	for (auto &l : lines) {
		if (l.compare(0, 4, "mask") == 0) {
			mask = l.substr(7);

		} else {
			uint64_t addressA;
			uint64_t addressB;
			uint64_t valueA;
			uint64_t valueB;

			if (sscanf(l.c_str(), "mem[%" SCNu64 "] = %" SCNu64, &addressA, &valueA) != 2) {
				abort();
			}

			std::vector<int> floatingBits;

			addressB = addressA;
			valueB   = valueA;

			for (int i = 0; i < 36; i++) {
				if (mask[i] == '0') {
					valueA &= ~(((uint64_t)1) << (35 - i));

				} else if (mask[i] == '1') {
					valueA   |= (((uint64_t)1) << (35 - i));
					addressB |= (((uint64_t)1) << (35 - i));

				} else {
					floatingBits.push_back(35 - i);
				}
			}

			memoryA[addressA] = valueA;

			{
				memoryB[addressB] = valueB;

				for (int i = 0; i < (1 << floatingBits.size()); i++) {
					uint64_t addr = addressB;

					for (int bit = 0; bit < floatingBits.size(); bit++) {
						if ((i & (1 << bit)) != 0) {
							addr |= (1ull << floatingBits[bit]);

						} else {
							addr &= ~(1ull << floatingBits[bit]);
						}
					}

					memoryB[addr] = valueB;
				}
			}
		}
	}

	uint64_t sumA = 0;
	uint64_t sumB = 0;

	for (auto &addr : memoryA) {
		sumA += addr.second;
	}

	for (auto &addr : memoryB) {
		sumB += addr.second;
	}

	PRINTF(("PART_A: %" PRIu64, sumA));
	PRINTF(("PART_B: %" PRIu64, sumB));

	return 0;
}
