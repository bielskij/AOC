#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <unistd.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class RepairRobot : public IntCodeMachine {
	public:
		static const int W = 128;
		static const int H = 128;

		enum Move {
			UNKNOWN = 0,
			NORTH   = 1,
			SOUTH   = 2,
			WEST    = 3,
			EAST    = 4
		};

		enum Status {
			HIT_THE_WALL    = 0,
			MOVED_TO_EMPTY  = 1,
			MOVED_TO_OXYGEN = 2
		};

		enum Tile {
			WALL,
			OXYGEN,
			EMPTY,
			NOT_VISITED
		};

	private:
		struct BfsElement {
			int x;
			int y;
			int distance;

			BfsElement(int x, int y) {
				this->x        = x;
				this->y        = y;
				this->distance = 0;
			}
		};

	public:
		RepairRobot(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			for (int y = 0; y < H; y++) {
				for (int x = 0; x < W; x++) {
					this->map[y][x] = Tile::NOT_VISITED;
				}
			}

			this->partA        = 0;
			this->moveBackward = false;
			this->position     = Point<int>(W / 2, H / 2);

			this->map[this->position.getY()][this->position.getX()] = Tile::EMPTY;
			this->mapVisited[this->position.getY()][this->position.getX()] = true;
		}

		bool onOut(int64_t value) {
			Point<int> newPosition = this->position;

			switch (this->history.top()) {
				case Move::EAST:  newPosition.setX(newPosition.getX() + (this->moveBackward ? -1 :  1)); break;
				case Move::WEST:  newPosition.setX(newPosition.getX() + (this->moveBackward ?  1 : -1)); break;
				case Move::SOUTH: newPosition.setY(newPosition.getY() + (this->moveBackward ? -1 :  1)); break;
				case Move::NORTH: newPosition.setY(newPosition.getY() + (this->moveBackward ?  1 : -1)); break;
			}

			if (this->moveBackward) {
				this->history.pop();
			}

			switch (value) {
				case Status::HIT_THE_WALL:
					{
						map[newPosition.getY()][newPosition.getX()] = Tile::WALL;

						this->history.pop();
					}
					break;

				case Status::MOVED_TO_EMPTY:
					{
						map[newPosition.getY()][newPosition.getX()] = Tile::EMPTY;

						this->position = newPosition;
					}
					break;

				case Status::MOVED_TO_OXYGEN:
					{
						map[newPosition.getY()][newPosition.getX()] = Tile::OXYGEN;

						this->position = newPosition;

						if (this->partA == 0) {
							this->partA      = this->history.size();
							this->partAPoint = this->position;
						}
					}
					break;
			}

//			this->draw();

			return true;
		}

		bool onIn(int64_t &value) {
			Move direction = Move::UNKNOWN;

			// UP
			if (direction == Move::UNKNOWN) {
				if (
					(this->position.getY() > 0) &&
					(! this->mapVisited[position.getY() - 1][position.getX()])
				) {
					this->mapVisited[position.getY() - 1][position.getX()] = true;

					direction = Move::NORTH;
				}
			}

			// East
			if (direction == Move::UNKNOWN) {
				if (
					(this->position.getX() + 1 < W) &&
					(! this->mapVisited[position.getY()][position.getX() + 1])
				) {
					this->mapVisited[position.getY()][position.getX() + 1] = true;

					direction = Move::EAST;
				}
			}

			// South
			if (direction == Move::UNKNOWN) {
				if (
					(this->position.getY() + 1 < H) &&
					(! this->mapVisited[position.getY() + 1][position.getX()])
				) {
					this->mapVisited[position.getY() + 1][position.getX()] = true;

					direction = Move::SOUTH;
				}
			}

			// West
			if (direction == Move::UNKNOWN) {
				if (
					(this->position.getX() > 0) &&
					(! this->mapVisited[position.getY()][position.getX() - 1])
				) {
					this->mapVisited[position.getY()][position.getX() - 1] = true;

					direction = Move::WEST;
				}
			}

			if (direction == Move::UNKNOWN) {
				if (this->history.empty()) {
					return false;
				}

				// Go back
				switch (this->history.top()) {
					case Move::EAST:  direction = Move::WEST;  break;
					case Move::WEST:  direction = Move::EAST;  break;
					case Move::NORTH: direction = Move::SOUTH; break;
					case Move::SOUTH: direction = Move::NORTH; break;
				}

				this->moveBackward = true;

			} else {
				this->history.push(direction);

				this->moveBackward = false;
			}

//			this->draw();

			value = direction;

			return true;
		}

		void draw() {
			printf("\e[1;1H\e[2J");

			for (int y = 0; y < H; y++) {
//				printf("     ");
				for (int x = 0; x < W; x++) {
					char c = ' ';

					if (this->position.getX() == x && this->position.getY() == y) {
						c = 'D';

					} else {
						switch (this->map[y][x]) {
							case Tile::WALL:        c = '#'; break;
							case Tile::OXYGEN:      c = 'O'; break;
							case Tile::EMPTY:       c = '.'; break;
							case Tile::NOT_VISITED: c = ' '; break;
						}
					}

					printf("%c", c);
				}
				printf("\n");
			}

			usleep(50 * 1000);
		}

		int getPartA() {
			return this->partA;
		}

		std::vector<Point<int>> _getNeighbors(const Point<int> &p) {
			std::vector<Point<int>> ret;

			Point<int> tmp;

			tmp = Point<int>(p.getX() + 1, p.getY());
			if (map[tmp.getY()][tmp.getX()] == Tile::EMPTY) {
				ret.push_back(tmp);
			}

			tmp = Point<int>(p.getX() - 1, p.getY());
			if (map[tmp.getY()][tmp.getX()] == Tile::EMPTY) {
				ret.push_back(tmp);
			}

			tmp = Point<int>(p.getX(), p.getY() + 1);
			if (map[tmp.getY()][tmp.getX()] == Tile::EMPTY) {
				ret.push_back(tmp);
			}

			tmp = Point<int>(p.getX(), p.getY() - 1);
			if (map[tmp.getY()][tmp.getX()] == Tile::EMPTY) {
				ret.push_back(tmp);
			}

			return ret;
		}

		int getPartB() {
			std::vector<Point<int>> paths;

			map[partAPoint.getY()][partAPoint.getX()] = Tile::NOT_VISITED;
			paths.push_back(this->partAPoint);

			int loops = 0;
			do {
				std::vector<Point<int>> newPaths;

				for (auto p = paths.begin(); p != paths.end(); p++) {
					auto children = _getNeighbors(*p);
					for (int i = 0; i < children.size(); i++) {
						newPaths.push_back(children[i]);
					}

					this->map[p->getY()][p->getX()] = Tile::NOT_VISITED;
				}

				paths = newPaths;

				if (! paths.empty()) {
					loops++;
				}

//				this->draw();
			} while (! paths.empty());

			return loops;
		}

	private:
		Point<int> position;
		Tile map[H][W];
		bool mapVisited[H][W];

		bool moveBackward;

		int        partA;
		Point<int> partAPoint;

		std::stack<Move> history;
};


int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		RepairRobot robot(program);

		robot.run();

		robot.draw();

		PRINTF(("PART_A: %d", robot.getPartA()));
		PRINTF(("PART_B: %d", robot.getPartB()));
	}
}
