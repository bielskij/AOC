#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int solve(const std::vector<std::string> &lines, int seqLength) {
	for (auto &l : lines) {
		for (int i = 0; i < l.length() - seqLength; i++) {
			std::string sub = l.substr(i, seqLength);

			std::set<char> chars = std::set<char>(sub.begin(), sub.end());

			if (chars.size() == seqLength) {
				return i + seqLength;
			}
		}
	}

	return -1;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, 4)));
	PRINTF(("PART_B: %d", solve(lines, 14)));

	return 0;
}
