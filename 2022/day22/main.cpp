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


static bool inTheSameSquare(const Point<int> &p1, const Point<int> &p2) {
	return (std::abs(p1.x() - p2.x()) < 50) && (std::abs(p1.y() - p2.y()) < 50);
}


static int getSquareIdx(const Point<int> &p) {
	return 3 * (p.y() / 50) + (p.x() / 50);
}


static int getSquareX(int squareIdx) {
	return (squareIdx % 3) * 50;
}


static int getSquareY(int squareIdx) {
	return (squareIdx / 3) * 50;
}


static int solve(std::vector<std::string> map, const Point<int> &startPoint, const Point<int> &max, const std::string &rules, bool partA) {
	int ret = 0;

	{
		Direction  currentDirection = Direction::RIGHT;
		Point<int> currentPoint     = startPoint;

		std::string toParse;

		for (auto idx = 0; idx < rules.size(); idx++) {
			Direction nextDirection    = currentDirection;
			bool      hasNextDirection = true;

			switch (rules[idx]) {
				case 'R':
					nextDirection = Direction::RIGHT;
					break;

				case 'L':
					nextDirection = Direction::LEFT;
					break;

				default:
					hasNextDirection = false;
					toParse += rules[idx];
			}

			if (hasNextDirection || (idx == rules.length() - 1)) {
				if (! toParse.empty()) {
					int steps = utils::toInt(toParse);

					toParse.clear();

					for (int step = 0; step < steps; step++) {
						auto mod = getDirectionModifier(currentDirection);

						Point<int> newCurrent = currentPoint + mod;

						if (! partA) {
							/*
							 * My map was:
							 *      ___ ___
							 *     |   |   |
							 *   0 | 1 | 2 |
							 *     |___|___|
							 *     |   |
							 *   3 | 4 | 5
							 *  ___|___|
							 * |   |   |
							 * | 6 | 7 | 8
							 * |___|___|
							 * |   |
							 * | 9 | 10  11
							 * |___|
							 */
							int srcSquare = getSquareIdx(currentPoint);

							if (newCurrent.x() < 0) {
								newCurrent.x(max.x() - 1);

							} else if (newCurrent.x() >= max.x()) {
								newCurrent.x(0);
							}

							if (newCurrent.y() < 0) {
								newCurrent.y(max.y() - 1);

							} else if (newCurrent.y() >= max.y()) {
								newCurrent.y(0);
							}

							if (getSquareIdx(newCurrent) == srcSquare) {
								if (! isWall(map[newCurrent.y()][newCurrent.x()])) {
									currentPoint = newCurrent;

									map[currentPoint.y()][currentPoint.x()] = directionToChar(currentDirection);
								}

							} else {
								Point<int> srcSquareOff = Point<int>(
									currentPoint.x() - getSquareX(srcSquare),
									currentPoint.y() - getSquareY(srcSquare)
								);

								Direction tmpDirection = currentDirection;

								switch (srcSquare) {
									case 1:
										switch (currentDirection) {
											case Direction::UP:
												newCurrent.set(getSquareX(9), getSquareY(9) + srcSquareOff.x());
												tmpDirection = Direction::RIGHT;
												break;

											case Direction::LEFT:
												newCurrent.set(getSquareX(6), getSquareY(6) + 49 - srcSquareOff.y());
												tmpDirection = Direction::RIGHT;
												break;

											default:
												break;
										}
										break;

									case 2:
										switch (currentDirection) {
											case Direction::UP:
												newCurrent.set(getSquareX(9) + srcSquareOff.x(), getSquareY(9) + 49);
												tmpDirection = Direction::UP;
												break;

											case Direction::DOWN:
												newCurrent.set(getSquareX(4) + 49, getSquareY(4) + srcSquareOff.x());
												tmpDirection = Direction::LEFT;
												break;

											case Direction::LEFT:
												break;

											case Direction::RIGHT:
												newCurrent.set(getSquareX(7) + 49, getSquareY(7) + 49 - srcSquareOff.y());
												tmpDirection = Direction::LEFT;
												break;
										}
										break;

									case 4:
										switch (currentDirection) {
											case Direction::UP:
											case Direction::DOWN:
												break;

											case Direction::LEFT:
												newCurrent.set(getSquareX(6) + srcSquareOff.y(), getSquareY(6));
												tmpDirection = Direction::DOWN;
												break;

											case Direction::RIGHT:
												newCurrent.set(getSquareX(2) + srcSquareOff.y(), getSquareY(2) + 49);
												tmpDirection = Direction::UP;
												break;
										}
										break;

									case 6:
										switch (currentDirection) {
											case Direction::UP:
												newCurrent.set(getSquareX(4), getSquareY(4) + srcSquareOff.x());
												tmpDirection = Direction::RIGHT;
												break;

											case Direction::LEFT:
												newCurrent.set(getSquareX(1), getSquareY(1) + 49 - srcSquareOff.y());
												tmpDirection = Direction::RIGHT;
												break;

											case Direction::DOWN:
											case Direction::RIGHT:
												break;
										}
										break;

									case 7:
										switch (currentDirection) {
											case Direction::UP:
											case Direction::LEFT:
												break;

											case Direction::DOWN:
												newCurrent.set(getSquareX(9) + 49, getSquareY(9) + srcSquareOff.x());
												tmpDirection = Direction::LEFT;
												break;

											case Direction::RIGHT:
												newCurrent.set(getSquareX(2) + 49, getSquareY(2) + 49 - srcSquareOff.y());
												tmpDirection = Direction::LEFT;
												break;
										}
										break;

									case 9:
										switch (currentDirection) {
											case Direction::UP:
												break;

											case Direction::DOWN:
												newCurrent.set(getSquareX(2) + currentPoint.x(), getSquareY(2));
												tmpDirection = Direction::DOWN;
												break;

											case Direction::LEFT:
												newCurrent.set(getSquareX(1) + srcSquareOff.y(), getSquareY(1));
												tmpDirection = Direction::DOWN;
												break;

											case Direction::RIGHT:
												newCurrent.set(getSquareX(7) + srcSquareOff.y(), getSquareY(7) + 49);
												tmpDirection = Direction::UP;
												break;
										}
										break;
								}

								if (isWall(map[newCurrent.y()][newCurrent.x()])) {
									break;
								}

								currentPoint     = newCurrent;
								currentDirection = tmpDirection;

								map[currentPoint.y()][currentPoint.x()] = directionToChar(currentDirection);
							}

						} else {
							if (
								(newCurrent.x() < 0 || newCurrent.x() >= max.x()) ||
								(newCurrent.y() < 0 || newCurrent.y() >= max.y()) ||
								map[newCurrent.y()][newCurrent.x()] == ' '
							) {
								switch (currentDirection) {
									case Direction::RIGHT:
										for (auto i = 0; i < max.x(); i++) {
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
										for (auto i = max.x() - 1; i >= 0; i--) {
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
										for (auto i = 0; i < max.y(); i++) {
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
										for (auto i = max.y() - 1; i >= 0; i--) {
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
				}

				if (hasNextDirection) {
					nextDirection = turn(currentDirection, nextDirection);
				}

				if (currentDirection != nextDirection) {
					currentDirection = nextDirection;

					map[currentPoint.y()][currentPoint.x()] = directionToChar(currentDirection);
				}
			}
		}

		ret = (currentPoint.y() + 1) * 1000 + (currentPoint.x() + 1) * 4 + currentDirection;
	}

	return ret;
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

		PRINTF(("PART_A: %lld", solve(map, startPoint, Point<int>(maxX, maxY), rules, true)));
		PRINTF(("PART_B: %lld", solve(map, startPoint, Point<int>(maxX, maxY), rules, false)));
	}
	return 0;
}
