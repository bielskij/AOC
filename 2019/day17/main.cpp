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


class ASCII : public IntCodeMachine {
	public:
		enum Mode {
			CALIBRATE_AREA,
			CALIBRATE_CAMERA,
			NONE,
			DUMP_MAP
		};

		enum Direction {
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		enum InputMode {
			CMD,
			A,
			B,
			C,
			VERBOSE
		};

	public:
		ASCII(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			this->maxX           = 0;
			this->maxY           = 0;
			this->mode           = CALIBRATE_AREA;
			this->cameraView     = nullptr;
			this->robotDirection = LEFT;
			this->inputMode      = InputMode::CMD;
			this->verbose        = 0;
			this->inputWritten   = 0;
		}

		bool onOut(int64_t value) {
			if (this->mode == Mode::CALIBRATE_AREA || this->mode == Mode::CALIBRATE_CAMERA) {
				// Store robot position and direction
				if (this->mode == Mode::CALIBRATE_CAMERA) {
					if (value == 'v') {
						this->robotPosition  = this->position;
						this->robotDirection = Direction::DOWN;

					} else if (value == '<') {
						this->robotPosition  = this->position;
						this->robotDirection = Direction::LEFT;

					} else if (value == '>') {
						this->robotPosition  = this->position;
						this->robotDirection = Direction::RIGHT;

					} else if (value == '^') {

						this->robotPosition  = this->position;
						this->robotDirection = Direction::UP;
					}
				}

				if (value == '\n') {
					this->position.y(this->position.y() + 1);

					if (this->mode == Mode::CALIBRATE_AREA) {
						if (this->position.x() > this->maxX) {
							this->maxX = this->position.x();
						}
					}

					this->position.x(0);

				} else {
					if (this->mode == Mode::CALIBRATE_CAMERA) {
						this->cameraView[this->position.y() * this->maxX + this->position.x()] = value;
					}

					this->position.x(this->position.x() + 1);
				}

			} else if (this->mode == Mode::DUMP_MAP) {
				if (value > 255) {
					PRINTF(("PART_B: %" PRId64, value));
				} else {
					putc(value, stdout);
				}
			}

			return true;
		}

		void draw() {
			printf("\e[1;1H\e[2J");

			for (int y = 0; y < this->maxY; y++) {
//				printf("     ");
				for (int x = 0; x < this->maxX; x++) {
					printf("%c", this->cameraView[y * this->maxX + x]);
				}
				printf("\n");
			}
		}

		bool onIn(int64_t &value) {
			switch (this->inputMode) {
				case InputMode::A:
					if (this->inputWritten != this->a.size()) {
						value = this->a[this->inputWritten++];

					} else {
						value = '\n';

						this->inputWritten = 0;
						this->inputMode    = InputMode::B;
					}
					break;

				case InputMode::B:
					if (this->inputWritten != this->b.size()) {
						value = this->b[this->inputWritten++];

					} else {
						value = '\n';

						this->inputWritten = 0;
						this->inputMode    = InputMode::C;
					}
					break;

				case InputMode::C:
					if (this->inputWritten != this->c.size()) {
						value = this->c[this->inputWritten++];

					} else {
						value = '\n';

						this->inputWritten = 0;
						this->inputMode    = InputMode::VERBOSE;
					}
					break;

				case InputMode::CMD:
					if (this->inputWritten != this->cmd.size()) {
						value = this->cmd[this->inputWritten++];

					} else {
						value = '\n';

						this->inputWritten = 0;
						this->inputMode    = InputMode::A;
					}
					break;

				case InputMode::VERBOSE:
					if (this->inputWritten != 1) {
						value = this->verbose;

						this->inputWritten++;

					} else {
						value = '\n';

						this->inputWritten = 0;
					}
					break;
			}

			return true;
		}

		void calibrateArea() {
			this->mode     = Mode::CALIBRATE_AREA;
			this->maxX     = 0;
			this->maxY     = 0;
			this->position = Point<int>(0, 0);

			this->reset();
			this->run();

			this->maxY = this->position.y();

			if (this->cameraView) {
				delete this->cameraView;
			}

			this->cameraView = new char[this->maxX * this->maxY];

			memset(this->cameraView, ' ', sizeof(char) * this->maxX * this->maxY);
		}

		void getPartA() {
			this->mode     = Mode::CALIBRATE_CAMERA;
			this->position = Point<int>(0, 0);

			this->reset();
			this->run();

			int sum = 0;

			for (int y = 1; y < this->maxY - 2; y++) {
				for (int x = 1; x < this->maxX - 2; x++) {
					if (
						(this->cameraView[y * this->maxX + x] == '#') &&

						(this->cameraView[(y - 1) * this->maxX + x] == '#') &&
						(this->cameraView[(y + 1) * this->maxX + x] == '#') &&

						(this->cameraView[y * this->maxX + x + 1] == '#') &&
						(this->cameraView[y * this->maxX + x - 1] == '#')
					) {
						sum += (x * y);
					}
				}
			}

			PRINTF(("PART_A: %d", sum));
		}

		Point<int> getNextPoint(const Point<int> &pos, Direction dir) {
			Point<int> ret = pos;

			switch (dir) {
				case Direction::DOWN:  ret.incY(); break;
				case Direction::UP:    ret.decY(); break;
				case Direction::LEFT:  ret.decX(); break;
				case Direction::RIGHT: ret.incX(); break;
			}

			if (
				(ret.x() >= this->maxX || ret.x() < 0) ||
				(ret.y() >= this->maxY || ret.y() < 0)
			) {
				ret = Point<int>(-1, -1);

			} else {
				if (this->cameraView[getViewOffset(ret)] != '#') {
					ret = Point<int>(-1, -1);
				}
			}

			return ret;
		}

		Direction getTurnDirection(const Point<int> &src, const Point<int> &dst, Direction dir) {
			Direction ret = Direction::LEFT;

			if (dst.y() > src.y()) {
				ret = (dir == Direction::RIGHT) ? Direction::RIGHT : Direction::LEFT;
			} else if (dst.y() < src.y()) {
				ret = (dir == Direction::RIGHT) ? Direction::LEFT : Direction::RIGHT;
			} else if (dst.x() > src.x()) {
				ret = (dir == Direction::UP) ? Direction::RIGHT : Direction::LEFT;
			} else if (dst.x() < src.x()) {
				ret = (dir == Direction::UP) ? Direction::LEFT : Direction::RIGHT;
			}

			return ret;
		}

		std::vector<Direction> getPossibleDirections(Direction dir) {
			if (dir == LEFT || dir == RIGHT) {
				return std::vector<Direction>({ UP, DOWN });

			} else {
				return std::vector<Direction>({ LEFT, RIGHT });
			}
		}

		int getViewOffset(const Point<int> &p) {
			return this->maxX * p.y() + p.x();
		}

		std::string getOperation(const std::string &op) {
			std::string sub;
			int         subOpBuffered = 0;

			for (int off = 0; off != op.size(); off++) {
				if (std::isdigit(op[off])) {
					sub += op[off];

					subOpBuffered++;

				} else {
					// letter
					if (subOpBuffered) {
						break;
					}

					sub += op[off];

					break;
				}
			}

			return sub;
		}

		void getPartB() {
			Direction  currentDirection = this->robotDirection;
			Point<int> currentPosition  = this->robotPosition;

			std::string operations;

			int moveSteps = 0;
			do {
				Point<int> nextPosition = getNextPoint(currentPosition, currentDirection);
				if (nextPosition.x() < 0) {
					if (moveSteps) {
						operations += utils::toString(moveSteps);

						moveSteps = 0;
					}

					auto possible = getPossibleDirections(currentDirection);
					auto dir = possible.begin();
					while (dir != possible.end()) {
						nextPosition = getNextPoint(currentPosition, *dir);

						if (nextPosition.x() >= 0) {
							switch (getTurnDirection(currentPosition, nextPosition, currentDirection)) {
								case LEFT:  operations += "L"; break;
								case RIGHT: operations += "R"; break;
							}

							currentDirection = *dir;
							break;
						}

						dir++;
					}

					if (dir == possible.end()) {
						break;
					}

				} else {
					moveSteps++;

					currentPosition = nextPosition;
				}
			} while (currentPosition.x() != -1);

			PRINTF(("Operations: %s", operations.c_str()));

			// TODO: Find an automatic method for finding  A, B, C commands (huffman codes?)
			this->a = "L,10,L,8,L,12,R,12";
			this->b = "L,12,L,8,R,12";
			this->c = "R,12,L,8,L,10";

			this->cmd = "B,A,B,C,B,B,C,A,C,A";

			this->reset();
			this->getMemory()[0] = 2;

			{
				this->inputMode    = InputMode::CMD;
				this->inputWritten = 0;

				this->mode    = Mode::DUMP_MAP;
				this->verbose = 'n';
			}

			this->run();
		}

	private:
		Mode mode;
		Point<int> position;
		int maxX;
		int maxY;

		char *cameraView;

		Point<int> robotPosition;
		Direction  robotDirection;

		std::string a;
		std::string b;
		std::string c;
		std::string cmd;

		InputMode inputMode;
		int       inputWritten;
		char      verbose;
};

int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		ASCII robot(program);

		robot.calibrateArea();
//		robot.draw();

		robot.getPartA();
		robot.getPartB();
	}
}
