#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int solve(std::vector<std::string> &group, int miss) {
	int ret = 0;

	for (int y = 1; y < group.size(); y++) {
		int u = y - 1;
		int l = y;
		int missmatch = 0;

		while (u >= 0 && l < group.size()) {
			int diff = 0;

			for (int i = 0; i < group[0].length(); i++) {
				if (group[u][i] != group[l][i]) {
					diff++;
				}
			}

			missmatch += diff;
			u--;
			l++;
		}

		if (missmatch == miss) {
			ret += (y * 100);
		}
	}

	for (int x = 1; x < group[0].length(); x++) {
		int l = x - 1;
		int r = x;
		int missmatch = 0;

		while (l >= 0 && r < group[0].length()) {
			int diff = 0;

			for (auto &row : group) {
				if (row[l] != row[r]) {
					diff++;
				}
			}

			missmatch += diff;
			l--;
			r++;
		}

		if (missmatch == miss) {
			ret += x;
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<std::string> group;

		int partA = 0;
		int partB = 0;

		for (int lineNo = 0; lineNo < lines.size(); lineNo++) {
			auto &l = lines[lineNo];

			bool process = false;

			if (l.empty()) {
				process = true;

			} else {
				group.push_back(l);

				if (lineNo == lines.size() - 1) {
					process = true;
				}
			}

			if (process) {
				partA += solve(group, 0);
				partB += solve(group, 1);

				group.clear();
			}
		}

		PRINTF(("PART_A: %d", partA));
		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
