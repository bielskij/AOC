#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int64_t solve(const std::vector<std::string> &lines, int multiplier) {
	std::map<int64_t, int64_t> insertedCols;
	std::map<int64_t, int64_t> insertedRows;

	{
		int64_t emptyCount = 0;

		for (int64_t y = 0; y < lines.size(); y++) {
			if (lines[y].find_first_not_of('.') == std::string::npos) {
				emptyCount++;

			} else {
				if (emptyCount) {
					insertedRows[y - emptyCount] = (emptyCount * multiplier) - emptyCount;

					emptyCount = 0;
				}
			}
		}

		for (int64_t x = 0; x < lines[0].size(); x++) {
			bool empty = true;

			for (auto &row : lines) {
				if (row[x] != '.') {
					empty = false;
					break;
				}
			}

			if (empty) {
				emptyCount++;

			} else {
				if (emptyCount) {
					insertedCols[x - emptyCount] = (emptyCount * multiplier) - emptyCount;

					emptyCount = 0;
				}
			}
		}
	}

	std::vector<Point<int64_t>> galaxies;

	for (int64_t y = 0; y < lines.size(); y++) {
		auto &row = lines[y];

		for (int64_t x = 0; x < row.length(); x++) {
			Point<int64_t> translated(x, y);

			if (row[x] == '.') {
				continue;
			}

			{
				auto it = insertedCols.begin();
				while (it->first < x && it != insertedCols.end()) {
					translated.x(translated.x() + it->second);
					++it;
				}
			}

			{
				auto it = insertedRows.begin();
				while (it->first < y && it != insertedRows.end()) {
					translated.y(translated.y() + it->second);
					++it;
				}
			}

			galaxies.push_back(translated);
		}
	}

	int64_t ret = 0;

	for (auto &src : galaxies) {
		for (auto &dst : galaxies) {
			if (dst != src) {
				ret += utils::manhattanDistance(src, dst);
			}
		}
	}

	return ret / 2;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);


	PRINTF(("PART_A: %" PRId64, solve(lines, 2)));
	PRINTF(("PART_B: %" PRId64, solve(lines, 1000000)));

	return 0;
}
