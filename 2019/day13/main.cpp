#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <queue>

#include <termios.h>
#include <unistd.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class ArcadeGame : public IntCodeMachine {
	private:
		const static int H = 32;
		const static int W = 64;

	public:
		enum OutState {
			OUT_STATE_X,
			OUT_STATE_Y,
			OUT_STATE_TILE
		};

		struct Tile {
			enum Type {
				TYPE_EMPTY,
				TYPE_WALL,
				TYPE_BLOCK,
				TYPE_HORIZONTAL_PADDLE,
				TYPE_BALL
			};

			Tile(Type type, const Point<int> &position) : position(position) {
				this->type = type;
			}

			Type       type;
			Point<int> position;
		};

	public:
		ArcadeGame(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			this->state    = OUT_STATE_X;
			this->score    = 0;
			this->simulate = false;

			for (int y = 0; y < H; y++) {
				for (int x = 0; x < W; x++) {
					this->tiles[y][x] = Tile::TYPE_EMPTY;
				}
			}
		}

		bool onOut(int64_t value) {
			switch (this->state) {
				case OUT_STATE_X:
					{
						this->tmp.setX(value);

						this->state = OUT_STATE_Y;
					}
					break;

				case OUT_STATE_Y:
					{
						this->tmp.setY(value);

						this->state = OUT_STATE_TILE;
					}
					break;

				case OUT_STATE_TILE:
					{
						if (this->tmp.getX() == -1) {
							this->score = value;

						} else {
							this->tiles[(int)this->tmp.getY()][(int)this->tmp.getX()] = (Tile::Type)value;

							if (value == Tile::TYPE_BALL) {
								this->ball = this->tmp;

							} else if (value == Tile::TYPE_HORIZONTAL_PADDLE) {
								this->paddle = this->tmp;
							}
						}

						this->state = OUT_STATE_X;
					}
					break;
			}

			return true;
		}

		bool onIn(int64_t &value) {
			if (this->simulate) {
				this->draw();
			}

			if (this->paddle.getX() > this->ball.getX()) {
				value = -1;
			} else if (this->paddle.getX() < this->ball.getX()) {
				value = 1;
			} else {
				value = 0;
			}

			if (this->simulate) {
				usleep(50 * 1000);
			}

			return true;
		}

		void onMemoryWrite(int64_t address, int64_t currentValue, int64_t newValue) {
			LOG(("onMemoryWrite [%" PRId64 "](%" PRId64 ") = %" PRId64, address, currentValue, newValue));
		}

		void onMemoryRead(int64_t address, int64_t currentValue) {
			LOG(("onMemoryRead [%" PRId64 "](%" PRId64 ")", address, currentValue));
		}

		int getTilesCount(Tile::Type type) {
			int ret = 0;

			for (int y = 0; y < H; y++) {
				for (int x = 0; x < W; x++) {
					if (this->tiles[y][x] == type) {
						ret++;
					}
				}
			}

			return ret;
		}

		int getScore() const {
			return this->score;
		}

		void draw() {
			printf("\e[1;1H\e[2J");

			for (int y = 0; y < H; y++) {
				printf("     ");
				for (int x = 0; x < W; x++) {
					char c = ' ';

					switch (this->tiles[y][x]) {
						case ArcadeGame::Tile::TYPE_BALL:              c = 'O'; break;
						case ArcadeGame::Tile::TYPE_BLOCK:             c = '#'; break;
						case ArcadeGame::Tile::TYPE_EMPTY:             c = ' '; break;
						case ArcadeGame::Tile::TYPE_HORIZONTAL_PADDLE: c = '-'; break;
						case ArcadeGame::Tile::TYPE_WALL:              c = '='; break;
					}

					printf("%c", c);
				}
				printf("\n");
			}

			printf("\nScore: %d\n", this->score);
		}

		bool run(bool simulate) {
			this->simulate = simulate;

			return this->IntCodeMachine::run();
		}

	private:
		Tile::Type tiles[H][W];
		OutState state;

		int score;
		Point<int> tmp;
		Point<int> ball;
		Point<int> paddle;
		bool simulate;
};

// app <path_to_data_file> <1|0 - simulate or not>
int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		ArcadeGame game(program);

		game.run(false);

		PRINTF(("PART_A: %d", game.getTilesCount(ArcadeGame::Tile::TYPE_BLOCK)));
	}

	{
		ArcadeGame game(program);

		game.getMemory()[0] = 2;

		game.run(utils::toInt(argv[2]));

		PRINTF(("PART_B: %d", game.getScore()));
	}
}
