#include <cassert>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


enum Direction {
	RIGHT,
	DOWN,
	LEFT,
	UP
};


static Direction turn(Direction current, Direction next) {
	switch (current) {
		case Direction::RIGHT:
			{
				switch (next) {
					case Direction::RIGHT: return Direction::DOWN;
					case Direction::LEFT:  return Direction::UP;
				}
			}
			break;

		case Direction::LEFT:
			{
				switch (next) {
					case Direction::RIGHT: return Direction::UP;
					case Direction::LEFT:  return Direction::DOWN;
				}
			}
			break;

		case Direction::UP:
			{
				switch (next) {
					case Direction::RIGHT: return Direction::RIGHT;
					case Direction::LEFT:  return Direction::LEFT;
				}
			}
			break;

		case Direction::DOWN:
			{
				switch (next) {
					case Direction::RIGHT: return Direction::LEFT;
					case Direction::LEFT:  return Direction::RIGHT;
				}
			}
			break;
	}

	assert(false);
	return Direction::RIGHT;
}


static Point<int> getDirectionModifier(Direction current) {
	Point<int> ret;

	switch (current) {
		case Direction::RIGHT: ret.set( 1,  0); break;
		case Direction::LEFT:  ret.set(-1,  0); break;
		case Direction::UP:    ret.set( 0, -1); break;
		case Direction::DOWN:  ret.set( 0,  1); break;

		default:
			assert(false);
	}

	return ret;
}


static char directionToChar(Direction dir) {
	switch (dir) {
		case Direction::RIGHT: return '>';
		case Direction::LEFT:  return '<';
		case Direction::UP:    return '^';
		case Direction::DOWN:  return 'v';
	}

	return '?';
}


static bool canWalk(char c) {
	switch (c) {
		case '.':
		case '>':
		case '<':
		case 'v':
		case '^':
			return true;
	}

	return false;
}


static bool isWall(char c) {
	return c == '#';
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int maxX = 0, maxY = 0;

		std::string rules;

		{
			bool parseRules = false;

			for (auto &l : lines) {
				if (! parseRules) {
					if (l.empty()) {
						parseRules = true;

					} else {
						if (l.length() > maxX) {
							maxX = l.length();
						}

						maxY++;
					}

				} else {
					rules = l;
					break;
				}
			}
		}

		std::vector<std::string> map;
		Point<int> startPoint(-1, -1);

		for (int y = 0 ; y < maxY; y++) {
			if (map.size() <= y) {
				map.push_back(std::string(maxX, ' '));
			}

			for (int x = 0; x < std::min((size_t) maxX, lines[y].length()); x++) {
				if (y == 0) {
					if (startPoint.x() < 0) {
						if (lines[y][x] == '.') {
							startPoint.set(x, y);
						}
					}
				}

				switch (lines[y][x]) {
					case '#':
					case '.':
						map[y][x] = lines[y][x];
						break;

					default:
						break;
				}
			}
		}

		{
			Direction  currentDirection = Direction::RIGHT;
			Point<int> currentPoint     = startPoint;

			std::string toParse;

			for (auto idx = 0; idx < rules.size(); idx++) {
				Direction nextDirection = currentDirection;

				switch (rules[idx]) {
					case 'R':
						nextDirection = turn(currentDirection, Direction::RIGHT);
						break;

					case 'L':
						nextDirection = turn(currentDirection, Direction::LEFT);
						break;

					default:
						toParse += rules[idx];
				}

				if ((currentDirection != nextDirection) || (idx == rules.length() - 1)) {
					if (! toParse.empty()) {
						int steps = utils::toInt(toParse);

						toParse.clear();

						auto mod = getDirectionModifier(currentDirection);

						for (int step = 0; step < steps; step++) {
							Point<int> newCurrent = currentPoint + mod;

							if (
								(newCurrent.x() < 0 || newCurrent.x() >= maxX) ||
								(newCurrent.y() < 0 || newCurrent.y() >= maxY) ||
								map[newCurrent.y()][newCurrent.x()] == ' '
							) {
								switch (currentDirection) {
									case Direction::RIGHT:
										for (auto i = 0; i < maxX; i++) {
											char c = map[currentPoint.y()][i];
											if (isWall(c)) {
												break;
											}
											if (canWalk(c)) {
												currentPoint.set(i, currentPoint.y());
												break;
											}
										}
										break;

									case Direction::LEFT:
										for (auto i = maxX - 1; i >= 0; i--) {
											auto c = map[currentPoint.y()][i];
											if (isWall(c)) {
												break;
											}
											if (canWalk(c)) {
												currentPoint.set(i, currentPoint.y());
												break;
											}
										}
										break;

									case Direction::DOWN:
										for (auto i = 0; i < maxY; i++) {
											auto c = map[i][currentPoint.x()];
											if (isWall(c)) {
												break;
											}
											if (canWalk(c)) {
												currentPoint.set(currentPoint.x(), i);
												break;
											}
										}
										break;

									case Direction::UP:
										for (auto i = maxY - 1; i >= 0; i--) {
											auto c = map[i][currentPoint.x()];
											if (isWall(c)) {
												break;
											}
											if (canWalk(c)) {
												currentPoint.set(currentPoint.x(), i);
												break;
											}
										}
										break;
								}

								map[currentPoint.y()][currentPoint.x()] = directionToChar(currentDirection);

							} else {
								if (canWalk(map[newCurrent.y()][newCurrent.x()])) {
									currentPoint = newCurrent;
									map[currentPoint.y()][currentPoint.x()] = directionToChar(currentDirection);
								}
							}
						}
					}

					if (currentDirection != nextDirection) {
						currentDirection = nextDirection;

						map[currentPoint.y()][currentPoint.x()] = directionToChar(currentDirection);
					}
				}
			}

#if 0
		printf("Start: %d, %d, mapSize: %d, %d\n", startPoint.x(), startPoint.y(), maxX + 1, maxY + 1);

		for (int y = 0; y < maxY; y++) {
			for (int x = 0; x < maxX; x++) {
//				if (y == (max))
			}
		}
		for (auto &r : map) {
			printf("'%s'\n", r.c_str());
		}

//		printf("\nRules: '%s'\n", rules.c_str());
#endif

			PRINTF(("PART_A: %lld", (currentPoint.y() + 1) * 1000 + (currentPoint.x() + 1) * 4 + currentDirection));
		}
	}
	return 0;
}
