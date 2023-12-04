#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static const std::vector<std::pair<std::string, int>> values = {
	{ "one",   1 },
	{ "two",   2 },
	{ "three", 3 },
	{ "four",  4 },
	{ "five",  5 },
	{ "six",   6 },
	{ "seven", 7 },
	{ "eight", 8 },
	{ "nine",  9 }
};


static int solve(const std::vector<std::string> &lines, bool partB) {
	int sum = 0;

	for (const auto &l : lines) {
		std::map<int, int> positions;

		for (int i = 0; i < l.length(); i++) {
			if (std::isdigit(l[i])) {
				positions[i] = l[i] - '0';

			} else if (partB) {
				for (const auto &d : values) {
					if (l.compare(i, d.first.length(), d.first) == 0) {
						positions[i] = d.second;

						i += (d.first.length() - 1);
						break;
					}
				}
			}
		}

		sum += (positions.begin()->second * 10 + positions.rbegin()->second);
	}

	return sum;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, false)));
	PRINTF(("PART_B: %d", solve(lines, true)));

	return 0;
}
