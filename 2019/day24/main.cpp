#include <algorithm>
#include <limits.h>
#include <cstring>
#include <unistd.h>

#include "common/types.h"

#include "utils/utils.h"
#include "utils/file.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


#define GRID_SIZE 403

struct Tile {
	static const int W = 5;
	static const int H = 5;

	char area[W * H];

	Tile(const std::vector<std::string> &data) {
		for (int y = 0; y < data.size(); y++) {
			for (int x = 0; x < data[y].length(); x++) {
				if (data[y][x] == '#') {
					this->set(x, y);
				} else {
					this->clear(x, y);
				}
			}
		}
	}

	Tile() {
		memset(area, '.', W * H);
	}

	Tile(const Tile &other) {
		memcpy(this->area, other.area, W * H);
	}

	void set(int x, int y) {
		this->area[y * W + x] = '#';
	}

	void clear(int x, int y) {
		this->area[y * W + x] = '.';
	}

	bool isBug(int x, int y) {
		if (x >= 0 && x < W && y >= 0 && y < H) {
			return this->area[y * W + x] == '#';

		} else {
			return false;
		}
	}

	void print() {
		printf("\n");
		for (int y = 0; y < H; y++) {
			for (int x = 0; x < W; x++) {
				printf("%c", area[y * W + x]);
			}
			printf("\n");
		}
	}

	std::string toString() {
		std::string ret;

		for (int y = 0; y < H; y++) {
			for (int x = 0; x < W; x++) {
				ret.push_back(this->area[y * W + x]);
			}
		}

		return ret;
	}

	Tile &operator=(const Tile &other) {
		memcpy(this->area, other.area, W * H);

		return *this;
	}
};


int main(int argc, char *argv[]) {
	Tile map(File::readAllLines(argv[1]));

	{
		std::map<std::string, int> combinations;

		Tile current = map;
		Tile tmp;

		bool cont = true;

		while (cont) {
//			current.print();

			combinations[current.toString()]++;

			for (auto &_comb : combinations) {
				if (_comb.second > 1) {
					long long int sum = 0;

					for (int i = 0; i < _comb.first.length(); i++) {
						if (_comb.first.at(i) == '#') {
							sum += (1 << (i));
						}
					}

					PRINTF(("PART_A: %lld", sum));

					cont = false;
					break;
				}
			}

			tmp = current;

			for (int y = 0; y < Tile::H; y++) {
				for (int x = 0; x < Tile::W; x++) {
					int bugsCount = 0;

					if (current.isBug(x - 1, y    )) bugsCount++;
					if (current.isBug(x + 1, y    )) bugsCount++;
					if (current.isBug(x    , y - 1)) bugsCount++;
					if (current.isBug(x    , y + 1)) bugsCount++;

					if (current.isBug(x, y)) {
						if (bugsCount != 1) {
							tmp.clear(x, y);
						}

					} else {
						if (bugsCount == 1 || bugsCount == 2) {
							tmp.set(x, y);
						}
					}
				}
			}

			current = tmp;
		}
	}

	{

	}
}
