#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<int> callories;

	{
		int c = 0;
		for (auto &l : lines) {
			if (l.empty()) {
				callories.push_back(c);
				c = 0;

			} else {
				c += std::stoi(l);
			}
		}

		std::sort(callories.rbegin(), callories.rend());

		PRINTF(("PART_A: %d", *callories.begin()));
	}

	{
		int sum = 0;

		for (int i = 0; i < 3; i++) {
			sum += callories[i];
		}

		PRINTF(("PART_B: %d", sum));
	}

	return 0;
}
