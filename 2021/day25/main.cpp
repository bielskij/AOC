#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		auto map = lines;

		int modified;
		int turns = 0;

		do {
			auto newMap = map;

			modified = 0;

			for (int y = 0; y < map.size(); y++) {
				std::string &row = map[y];

				for (int x = 0; x < row.length(); x++) {
					if (row[x] == '>') {
						int nextCol = (x + 1 < row.length() ? x + 1 : 0);

						if (row[nextCol] == '.') {
							newMap[y][nextCol] = '>';
							newMap[y][x] = '.';

							modified++;
						}
					}
				}
			}

			map = newMap;

			for (int y = 0; y < map.size(); y++) {
				std::string &row = map[y];

				for (int x = 0; x < row.length(); x++) {
					if (row[x] == 'v') {
						int nextRow = (y + 1 < map.size() ? y + 1 : 0);

						if (map[nextRow][x] == '.') {
							newMap[nextRow][x] = 'v';
							newMap[y][x]       = '.';

							modified++;
						}
					}
				}
			}

			map = newMap;

			turns++;

		} while (modified > 0);

		PRINTF(("PART_A: %d", turns));
	}

	return 0;
}
