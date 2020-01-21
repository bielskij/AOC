#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	std::string resultA;
	std::string resultB;

	for (int col = 0; col < data[0].size(); col++) {
		std::map<char, int> letterMap;

		for (int row = 0; row < data.size(); row++) {
			letterMap[data[row][col]]++;
		}

		int max  = std::numeric_limits<int>::min();
		int min  = std::numeric_limits<int>::max();
		int minC = 0;
		int maxC = 0;

		for (auto &c : letterMap) {
			if (c.second > max) {
				max  = c.second;
				maxC = c.first;
			}

			if (c.second < min) {
				min  = c.second;
				minC = c.first;
			}
		}

		resultA += maxC;
		resultB += minC;
	}

	PRINTF(("PART_A: '%s'", resultA.c_str()));
	PRINTF(("PART_B: '%s'", resultB.c_str()));
}
