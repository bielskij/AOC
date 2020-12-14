#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"



static int countOccupiedSeats(const std::vector<std::string> &map, int col, int row, bool partA) {
	int ret = 0;

	if (partA) {
		int minX, maxX, minY, maxY;

		minX = col; if (col > 0) minX--;
		maxX = col; if (col < map[row].size() - 1) maxX++;

		minY = row; if (row > 0) minY--;
		maxY = row; if (row < map.size() - 1) maxY++;

		for (int y = minY; y <= maxY; y++) {
			for (int x = minX; x <= maxX; x++) {
				if (y == row && x == col) {
					continue;
				}

				if (map[y][x] == '#') {
					ret++;
				}
			}
		}

	} else {
		int modX[8] = { -1,  0,  1, 1, 1, 0, -1, -1 };
		int modY[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };

		for (int m = 0; m < 8; m++) {
			int x = col + modX[m];
			int y = row + modY[m];

			bool found = false;

			while (x >= 0 && x < map[y].size() && y >= 0 && y < map.size()) {
				if (map[y][x] == '#') {
					found = true;
					break;

				} else if (map[y][x] == 'L') {
					break;
				}

				x += modX[m];
				y += modY[m];
			}

			if (found) {
				ret++;
			}
		}
	}

	return ret;
}


static int solve(const std::vector<std::string> &map, bool partA) {
	int ret = 0;

	{
		std::vector<std::string> currentMap = map;

		bool changed = false;

		do {
			std::vector<std::string> newMap = currentMap;

			for (auto row = 0; row < newMap.size(); row++) {
				for (auto col = 0; col < newMap[row].size(); col++) {
					int occupied = countOccupiedSeats(currentMap, col, row, partA);

					switch (currentMap[row][col]) {
						case '.':
							break;

						case 'L':
							if (occupied == 0) {
								newMap[row][col] = '#';
							}
							break;

						case '#':
							{
								int val = 4;

								if (! partA) {
									val = 5;
								}

								if (occupied >= val) {
									newMap[row][col] = 'L';
								}
							}
							break;
					}
				}
			}

			changed = newMap != currentMap;

			currentMap = newMap;
		} while (changed);

		for (auto &row : currentMap) {
			ret += std::count(row.begin(), row.end(), '#');
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, true)));
	PRINTF(("PART_B: %d", solve(lines, false)));

	return 0;
}
