#include <cassert>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int solve(const std::vector<std::string> &input, int xMod, bool print) {
	std::vector<std::vector<Point<int>>> rocks;

	enum State {
		STATE_PARSE_POINT_X,
		STATE_PARSE_POINT_Y,
	} state = STATE_PARSE_POINT_X;

	Point<int> p;

	for (auto &l : input) {
		int val = 0;

		std::vector<Point<int>> path;


		state = STATE_PARSE_POINT_X;

		for (char c : l) {
			switch (state) {
				case STATE_PARSE_POINT_X:
					if (std::isdigit(c)) {
						val *= 10;
						val += c - '0';

					} else if (c == ',') {
						p.x(val);

						val   = 0;
						state = STATE_PARSE_POINT_Y;
					}
					break;

				case STATE_PARSE_POINT_Y:
					if (std::isdigit(c)) {
						val *= 10;
						val += c - '0';

					} else if (c == '>') {
						p.y(val);

						path.push_back(p);

						val   = 0;
						state = STATE_PARSE_POINT_X;
					}
					break;
			}
		}

		if (state == STATE_PARSE_POINT_Y) {
			p.y(val);

			path.push_back(p);
		}

		rocks.push_back(path);
	}

	{
		Point<int> start(500, 0);

		int minX = start.x();
		int maxX = start.x();
		int minY = start.y();
		int maxY = start.y();

		for (auto &path : rocks) {
			for (auto &point : path) {
				if (point.x() > maxX) {
					maxX = point.x();
				}

				if (point.x() < minX) {
					minX = point.x();
				}

				if (point.y() > maxY) {
					maxY = point.y();
				}

				if (point.y() < minY) {
					minY = point.y();
				}
			}
		}

		int modX = xMod;
		int modY = 3;

		int width  = maxX - minX + 1 + modX;
		int height = maxY - minY + 1 + modY;

		minX -= (modX / 2);
		maxX -= (modX / 2);
		maxY += (modY);

		std::vector<std::string> map;

		for (int y = 0; y < height; y++) {
			map.push_back(std::string(width, '.'));
		}

		for (auto &path : rocks) {
			auto &src = path[0];

			for (size_t i = 1; i < path.size(); i++) {
				auto &dst = path[i];

				if (src.x() != dst.x()) {
					Point<int> start = src.x() > dst.x() ? dst : src;
					Point<int> end   = src.x() > dst.x() ? src : dst;

					for (int x = start.x(); x <= end.x(); x++) {
						map[start.y() - minY][x - minX] = '#';
					}

				} else if (src.y() != dst.y()) {
					Point<int> start = src.y() > dst.y() ? dst : src;
					Point<int> end   = src.y() > dst.y() ? src : dst;

					for (int y = start.y(); y <= end.y(); y++) {
						map[y - minY][start.x() - minX] = '#';
					}

				} else {
					assert(false);
				}

				src = dst;
			}
		}

		map[start.y() - minY][start.x() - minX] = '+';

		for (int x = 0; x < width; x++) {
			map[height - 2][x] = '#';
		}

		std::vector<Point<int>> sand;

		bool finished = false;
		Point<int> *unit = nullptr;
		do {
			bool added = false;

			if (unit == nullptr) {
				sand.push_back(Point<int>(start.x() - minX, start.y() - minY));

				unit = &*sand.rbegin();

				added = true;
			}

			if (unit->x() <= 0 || unit->x() >= (width - 1) || unit->y() >= (height - 1)) {
				if (print) {
					for (int y = 0; y < height; y++) {
						for (int x = 0; x < width; x++) {
							printf("%c", map[y][x]);
						}
						printf("\n");
					}
				}
				finished = true;

			} else {
				if (map[unit->y() + 1][unit->x()] == '.') {
					unit->incY();

				} else if (map[unit->y() + 1][unit->x() - 1] == '.') {
					unit->incY();
					unit->decX();

				} else if (map[unit->y() + 1][unit->x() + 1] == '.') {
					unit->incY();
					unit->incX();

				} else {
					if (*unit == Point<int>(start.x() - minX, start.y() - minY)) {
						if (print) {
							for (int y = 0; y < height; y++) {
								for (int x = 0; x < width; x++) {
									printf("%c", map[y][x]);
								}
								printf("\n");
							}
						}

						finished = true;
					} else {

						map[unit->y()][unit->x()] = 'o';
					}

					unit = nullptr;
				}
			}

		} while (! finished);

		return sand.size();
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, 2, true) - 1));
	PRINTF(("PART_B: %d", solve(lines, 512, false)));

	return 0;
}
