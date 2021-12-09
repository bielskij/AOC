#include <queue>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


std::vector<Point<int>> getAdiacents(int width, int height, int x, int y) {
	std::vector<Point<int>> adjacents;

	if (x > 0) {
		adjacents.push_back(Point<int>(x - 1, y));
	}

	if (x < width - 1) {
		adjacents.push_back(Point<int>(x + 1, y));
	}

	if (y > 0) {
		adjacents.push_back(Point<int>(x, y - 1));
	}

	if (y < height - 1) {
		adjacents.push_back(Point<int>(x, y + 1));
	}

	return adjacents;
}


std::vector<Point<int>> getAdiacents(int width, int height, const Point<int> &p) {
	return getAdiacents(width, height, p.x(), p.y());
}


static uint8_t **allocMap(int width, int height) {
	uint8_t **ret = new uint8_t*[height];

	for (int i = 0; i < height; i++) {
		ret[i] = new uint8_t[width]();
	}

	return ret;
}


static void freeMap(uint8_t **map, int height) {
	for (int i = 0; i < height; i++) {
		delete [] map[i];
	}

	delete [] map;
}


enum State {
	NONE,
	VISITED,
	LOWEST,
	BASIN
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int width  = lines.begin()->length();
		int height = lines.size();

		uint8_t **heightMap = allocMap(width, height);
		uint8_t **resultMap = allocMap(width, height);

		for (int y = 0; y < lines.size(); y++) {
			const auto &row = lines[y];

			for (int x = 0; x < row.size(); x++) {
				heightMap[y][x] = row[x] - '0';
			}
		}

		{
			std::vector<Point<int>> lowestPoints;
			std::vector<int> basins;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					bool isLowest = true;

					for (const auto &adj : getAdiacents(width, height, x, y)) {
						if (heightMap[adj.y()][adj.x()] <= heightMap[y][x]) {
							isLowest = false;
							break;
						}
					}

					if (isLowest) {
						lowestPoints.push_back(Point<int>(x, y));

						resultMap[y][x] = State::LOWEST;
					}
				}
			}

			for (const auto &p : lowestPoints) {
				int basinSize = 1;

				std::queue<Point<int>> queue;

				queue.push(p);

				while (! queue.empty()) {
					Point<int> p = queue.front(); queue.pop();

					for (const auto &adj : getAdiacents(width, height, p)) {
						if (resultMap[adj.y()][adj.x()] == State::NONE) {
							resultMap[adj.y()][adj.x()] = State::VISITED;

							if (heightMap[adj.y()][adj.x()] < 9) {
								basinSize++;

								queue.push(adj);

								resultMap[adj.y()][adj.x()] = State::BASIN;
							}
						}
					}
				}

				basins.push_back(basinSize);
			}

			{
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						char c;
						std::string colorDef = "\x1B[0m";
						std::string color;

						switch (resultMap[y][x]) {
							case State::LOWEST: c = 'O'; break;
							case State::BASIN:  c = ' '; break;

							default:
								c = ' ';
								break;
						}

						switch (heightMap[y][x]) {
							case 0: color = "\x1B[48;5;16m"; break;
							case 1: color = "\x1B[48;5;18m"; break;
							case 2: color = "\x1B[48;5;20m"; break;
							case 3: color = "\x1B[48;5;21m"; break;
							case 4: color = "\x1B[48;5;27m"; break;
							case 5: color = "\x1B[48;5;33m"; break;
							case 6: color = "\x1B[48;5;39m"; break;
							case 7: color = "\x1B[48;5;45m"; break;
							case 8: color = "\x1B[48;5;51m"; break;
							case 9: color = "\x1B[48;5;159m"; break;
						}

						printf("%s%c%s", color.c_str(), c, colorDef.c_str());
					}

					printf("\n");
				}
				printf("\n");
			}

			{
				int partA = 0;

				for (const auto &p : lowestPoints) {
					partA += (heightMap[p.y()][p.x()] + 1);
				}

				PRINTF(("PART_A: %d", partA));
			}

			{
				int partB = 1;

				std::sort(basins.begin(), basins.end());

				auto it = basins.rbegin();
				partB *= *it;

				it = std::next(it);
				partB *= *it;

				it = std::next(it);
				partB *= *it;

				PRINTF(("PART_B: %d", partB));
			}
		}

		freeMap(heightMap, height);
		freeMap(resultMap, height);
	}

	return 0;
}
