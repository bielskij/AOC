#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int8_t **allocMap(int width, int height) {
	int8_t **ret = new int8_t*[height];

	for (int i = 0; i < height; i++) {
		ret[i] = new int8_t[width]();
	}

	return ret;
}


static void freeMap(int8_t **map, int height) {
	for (int i = 0; i < height; i++) {
		delete [] map[i];
	}

	delete [] map;
}


std::vector<Point<int>> getAdiacents(int width, int height, int x, int y) {
	std::vector<Point<int>> adjacents;

	adjacents.push_back(Point<int>(x - 1, y - 1));
	adjacents.push_back(Point<int>(x - 1, y));
	adjacents.push_back(Point<int>(x - 1, y + 1));

	adjacents.push_back(Point<int>(x, y - 1));
	adjacents.push_back(Point<int>(x, y + 1));

	adjacents.push_back(Point<int>(x + 1, y - 1));
	adjacents.push_back(Point<int>(x + 1, y));
	adjacents.push_back(Point<int>(x + 1, y + 1));

	auto it = adjacents.begin();

	while (it != adjacents.end()) {
		if (it->x() < 0 || it->x() >= width) {
			it = adjacents.erase(it);

		} else if (it->y() < 0 || it->y() >= height) {
			it = adjacents.erase(it);

		} else {
			it++;
		}
	}

	return adjacents;
}


std::vector<Point<int>> getAdiacents(int width, int height, const Point<int> &p) {
	return getAdiacents(width, height, p.x(), p.y());
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int width  = lines.begin()->length();
		int height = lines.size();

		int8_t **octopusMap = allocMap(width, height);

		for (int y = 0; y < lines.size(); y++) {
			const auto &row = lines[y];

			for (int x = 0; x < row.size(); x++) {
				octopusMap[y][x] = row[x] - '0';
			}
		}

		int turn = 0;
		int totalFlashing = 0;

		while (1) {
			if (turn == utils::toInt(argv[2])) {
				PRINTF(("PART_A: %d", totalFlashing));
			}

			// step 1. Increase energy level
			for (int y = 0; y < lines.size(); y++) {
				const auto &row = lines[y];

				for (int x = 0; x < row.size(); x++) {
					octopusMap[y][x]++;
				}
			}

			// step 2. apply energy level on all adjacents
			{
				int newFlashing;

				do {
					newFlashing = 0;

					for (int y = 0; y < lines.size(); y++) {
						const auto &row = lines[y];

						for (int x = 0; x < row.size(); x++) {
							if (octopusMap[y][x] > 9) {
								std::vector<Point<int>> adjacents = getAdiacents(width, height, x, y);

								octopusMap[y][x] = -1; newFlashing++;

								for (const auto &adj : adjacents) {
									int8_t &octopus = octopusMap[adj.y()][adj.x()];

									if (octopus != -1) {
										octopus++;
									}
								}
							}
						}
					}

					totalFlashing += newFlashing;
				} while (newFlashing > 0);
			}

			// Step 3, update energy level of flashing octopuses
			for (int y = 0; y < lines.size(); y++) {
				const auto &row = lines[y];

				for (int x = 0; x < row.size(); x++) {
					if (octopusMap[y][x] == -1) {
						octopusMap[y][x] = 0;
					}
				}
			}

			{
				int sum = 0;

				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						sum += octopusMap[y][x];
					}
				}

				if (sum == 0) {
					PRINTF(("PART_B: %d", turn + 1));
					break;
				}
			}

			turn++;
		}

		freeMap(octopusMap, height);
	}

	return 0;
}
