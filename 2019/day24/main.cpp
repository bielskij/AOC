#include <algorithm>
#include <limits.h>
#include <cstring>
#include <unistd.h>

#include "common/types.h"

#include "utils/utils.h"
#include "utils/file.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


static bool isBug(char *map, int x, int y, int w, int h) {
	if (x >= 0 && x < w && y >= 0 && y < h) {
		return map[y * w + x] == '#';
	}

	return false;
}


static void printMap(char *map, int w, int h) {
	printf("\n");
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			printf("%c", map[y * w + x]);
		}
		printf("\n");
	}
}


int main(int argc, char *argv[]) {
	std::vector<std::string> data = File::readAllLines(argv[1]);

	const int w = 5;
	const int h = 5;

	char map[w * h];
	char newMap[w * h];

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			map[y * w + x] = data[y][x];
		}
	}

	std::map<std::string, int> combinations;

	std::string c;
//	c.reserve(w * h);

	bool cont = true;

	while (cont) {
//		printMap(map, w, h);

		c.clear();

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				c.push_back(map[y * w + x]);
			}
		}

		combinations[c]++;

		for (auto &_comb : combinations) {
			if (_comb.second > 1) {
				long long int sum = 0;

				for (int i = 0; i < _comb.first.length(); i++) {
					if (_comb.first.at(i) == '#') {
						sum += (1 << (i));
					}
				}

				PRINTF(("PART_A: %lld", sum));

				cont = false;
				break;
			}
		}

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int bugsCount = 0;

				if (isBug(map, x - 1, y, w, h)) {
					bugsCount++;
				}

				if (isBug(map, x + 1, y, w, h)) {
					bugsCount++;
				}

				if (isBug(map, x, y - 1, w, h)) {
					bugsCount++;
				}

				if (isBug(map, x, y + 1, w, h)) {
					bugsCount++;
				}

				newMap[y * w + x] = map[y * w + x];

				if (isBug(map, x, y, w, h)) {
					if (bugsCount != 1) {
						newMap[y * w + x] = '.';
					}

				} else {
					if (bugsCount == 1 || bugsCount == 2) {
						newMap[y * w + x] = '#';
					}
				}
			}
		}

		memcpy(map, newMap, sizeof(map));
	}
}
