#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <queue>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Pixel {
	Point<int> position;
	int        color;
	int        paintNumber;

	static int DEFAULT_COLOR;

	Pixel() {
		color       = DEFAULT_COLOR;
		paintNumber = 0;
	}

	void setColor(int color) {
		this->color = color;

		this->paintNumber++;
	}
};

int Pixel::DEFAULT_COLOR = 0;


class Robot : public IntCodeMachine {
	public:
		static const int BLACK = 0;
		static const int WHITE = 1;

	private:
		static const int TURN_LEFT  = 0;
		static const int TURN_RIGHT = 1;

	private:
		enum Mode {
			COLOR,
			DIRECTION
		};

		enum Direction {
			UP,
			DOWN,
			LEFT,
			RIGHT
		};

	public:
		Robot(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			this->mode      = COLOR;
			this->direction = UP;
		}

		bool onOut(int64_t value) {
			switch (this->mode) {
				case COLOR:
					{
						this->mode = DIRECTION;

						this->setColor(value);
					}
					break;

				case DIRECTION:
					{
						this->mode = COLOR;

						switch(this->direction) {
							case UP:
								{
									if (value == TURN_LEFT) {
										this->direction = LEFT;
									} else {
										this-> direction = RIGHT;
									}
								}
								break;

							case DOWN:
								{
									if (value == TURN_LEFT) {
										this->direction = RIGHT;
									} else {
										this->direction = LEFT;
									}
								}
								break;

							case LEFT:
								{
									if (value == TURN_LEFT) {
										this->direction = DOWN;
									} else {
										this->direction = UP;
									}
								}
								break;

							case RIGHT:
								{
									if (value == TURN_LEFT) {
										this->direction = UP;
									} else {
										this->direction = DOWN;
									}
								}
								break;
						}

						this->moveForward();
					}
					break;
			}

			return true;
		}

		bool onIn(int64_t &value) {
			value = this->getColor();

			return true;
		}

		void moveForward() {
			switch (this->direction) {
				case UP:
					this->position.y(this->position.y() - 1);
					break;

				case DOWN:
					this->position.y(this->position.y() + 1);
					break;

				case LEFT:
					this->position.x(this->position.x() - 1);
					break;

				case RIGHT:
					this->position.x(this->position.x() + 1);
					break;
			}
		}

		int getColor() {
			return this->getPixel()->color;
		}

		void setColor(int color) {
			this->getPixel()->setColor(color);
		}

		Pixel *getPixel() {
			Pixel *ret = NULL;

			for (auto p = this->pixels.begin(); p != this->pixels.end(); p++) {
				if (p->position == this->position) {
					ret = &(*p);
					break;
				}
			}

			if (ret == NULL) {
				Pixel p;

				p.position = this->position;

				this->pixels.push_back(p);

				ret = &(*this->pixels.rbegin());
			}

			return ret;
		}

		const std::vector<Pixel> &getPixels() {
			return this->pixels;
		}

	private:
		Mode       mode;
		Direction  direction;
		Point<int> position;

		std::vector<Pixel> pixels;
};


int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		Robot robot(program);

		robot.run();

		const std::vector<Pixel> &pixels = robot.getPixels();

		int sum = 0;

		for (auto p = pixels.begin(); p != pixels.end(); p++) {
			if (p->paintNumber > 0) {
				sum++;
			}
		}

		PRINTF(("PART_A: %d", sum));
	}

	{
		Pixel::DEFAULT_COLOR = 1;

		Robot robot(program);

		robot.run();

		const std::vector<Pixel> &pixels = robot.getPixels();

		{
			int xMin = INT_MAX;
			int yMin = INT_MAX;
			int xMax = INT_MIN;
			int yMax = INT_MIN;

			for (auto p = pixels.begin(); p != pixels.end(); p++) {
				if (p->position.x() < xMin) {
					xMin = p->position.x();
				}

				if (p->position.x() > xMax) {
					xMax = p->position.x();
				}

				if (p->position.y() < yMin) {
					yMin = p->position.y();
				}

				if (p->position.y() > yMax) {
					yMax = p->position.y();
				}
			}

			int xOffset = xMin;
			int yOffset = yMin;
			int width   = xMax - xMin + 1;
			int height  = yMax - yMin + 1;

			int map[height][width];

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					map[y][x] = 0;
				}
			}

			for (auto p = pixels.begin(); p != pixels.end(); p++) {
				map[(int)(p->position.y() - yOffset)][(int)(p->position.x() - xOffset)] = p->color;
			}

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					printf("%c", map[y][x] == 1 ? '#' : ' ');
				}
				printf("\n");
			}
		}
	}
}
