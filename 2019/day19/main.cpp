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


class Drone : public IntCodeMachine {
	public:
		Drone(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			this->inY = false;
			this->out = 0;
		}

		bool onOut(int64_t value) {
			this->out = value;

			return true;
		}

		bool onIn(int64_t &value) {
			value = this->in[this->inY];

			this->inY = ! this->inY;

			return true;
		}

		void getLineCoords(int y, Point<int> &start, Point<int> &end) {
			bool hasStart = false;
			bool hasEnd   = false;

			int x = 0;

			do {
				this->in[0] = x++;
				this->in[1] = y;

				this->reset();
				this->run();

				if (! hasStart) {
					if (this->out == 1) {
						start.set(x - 1, y);

						hasStart = true;
					}

				} else if (! hasEnd) {
					if (this->out == 0) {
						end.set(x - 2, y);

						hasEnd = true;
					}
				}
			} while (! hasStart || ! hasEnd);
		}

		void getPartA(bool verbose) {
			int sum = 0;
			int w   = 50;
			int h   = 50;

			if (verbose) {
				printf("\e[1;1H\e[2J");
			}

			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					this->in[0] = x;
					this->in[1] = y;

					this->reset();
					this->run();

					if (verbose) {
						printf("%c", this->out == 0 ? '.' : '#');
					}

					sum += this->out;
				}
				if (verbose) {
					printf("\n");
				}
			}

			PRINTF(("PART_A: %d", sum));
		}

		void getPartB() {
			int expectedSquareLength = 100;

			Point<int> startA, endA, startB, endB;

			// Raw estimate the nearest point
			for (int i = 10; i < 10000; i += 50) {
				getLineCoords(i                           , startA, endA);
				getLineCoords(i + expectedSquareLength - 1, startB, endB);

				if (endA.x() - startB.x() + 1 >= expectedSquareLength) {
					break;
				}
			}

			// Find the exact point
			{
				Point<int> tmpStartA, tmpEndA, tmpStartB, tmpEndB;

				int startY = startA.y();

				for (int i = startY; i > 0; i--) {
					getLineCoords(i                           , tmpStartA, tmpEndA);
					getLineCoords(i + expectedSquareLength - 1, tmpStartB, tmpEndB);

					if (tmpEndA.x() - tmpStartB.x() + 1 >= expectedSquareLength) {
						startA = tmpStartA;
						endA   = tmpEndA;
						startB = tmpStartB;
						endB   = tmpEndB;

					} else {
						break;
					}
				}
			}

			PRINTF(("PART_B: %d", 10000 * (startB.x()) + startA.y()));
		}

	private:
		int in[2];
		int out;
		bool inY;
};

int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		Drone robot(program);

		robot.getPartA(utils::toInt(argv[2]));
		robot.getPartB();
	}
}
