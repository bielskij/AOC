#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int getNeighbors(std::vector<std::string> &map, int _x, int _y) {
	int ret = 0;

	for (int y = _y - 1; y <= _y + 1; y++) {
		for (int x = _x - 1; x <= _x + 1; x++) {
			if (x == _x && y == _y) {
				continue;
			}

			if (x < 0 || y < 0 || y >= map.size() || x >= map.at(y).size()) {
				continue;
			}

			if (map[y][x] == '#') {
				ret++;
			}
		}
	}

	return ret;
}


static int _gameOfLife(std::vector<std::string> &data, int loops, bool cornersAlwaysOn) {
	int ret = 0;

	std::vector<std::string> map = data;

	for (int l = 0; l < loops; l++) {
		auto newMap = map;

		if (cornersAlwaysOn) {
			map[             0][                0] = '#';
			map[map.size() - 1][                0] = '#';
			map[             0][map[0].size() - 1] = '#';
			map[map.size() - 1][map[0].size() - 1] = '#';
		}

		for (int y = 0; y < map.size(); y++) {
			for (int x = 0; x < map[y].length(); x++) {
				int neighbors = getNeighbors(map, x, y);

				if (map[y][x] == '#') {
					newMap[y][x] = '.';
					if (neighbors == 2 || neighbors == 3) {
						newMap[y][x] = '#';
					}

				} else {
					newMap[y][x] = '.';
					if (neighbors == 3) {
						newMap[y][x] = '#';
					}
				}
			}
		}

		map = newMap;

		if (cornersAlwaysOn) {
			map[             0][                0] = '#';
			map[map.size() - 1][                0] = '#';
			map[             0][map[0].size() - 1] = '#';
			map[map.size() - 1][map[0].size() - 1] = '#';
		}
	}

	for (int y = 0; y < map.size(); y++) {
		for (int x = 0; x < map[y].length(); x++) {
			if (map[y][x] == '#') {
				ret++;
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", _gameOfLife(data, utils::toInt(argv[2]), false)));
	PRINTF(("PART_B: %d", _gameOfLife(data, utils::toInt(argv[2]), true)));
}
