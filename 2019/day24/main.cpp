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


int getBugsCount(Tile grid[GRID_SIZE], Tile &bkpGrid, int index, int x, int y) {
	int bugsCount = 0;

	if (! (y == 2 && x == 2)) {
		if (y == 0 && x == 0) {
			if (bkpGrid.isBug(2, 1))     bugsCount++;
			if (bkpGrid.isBug(1, 2))     bugsCount++;
			if (grid[index].isBug(1, 0)) bugsCount++;
			if (grid[index].isBug(0, 1)) bugsCount++;

		} else if (y == 0 && x == 4) {
			if (bkpGrid.isBug(2, 1))     bugsCount++;
			if (bkpGrid.isBug(3, 2))     bugsCount++;
			if (grid[index].isBug(3, 0)) bugsCount++;
			if (grid[index].isBug(4, 1)) bugsCount++;

		} else if (y == 4 && x == 0) {
			if (bkpGrid.isBug(2, 3))     bugsCount++;
			if (bkpGrid.isBug(1, 2))     bugsCount++;
			if (grid[index].isBug(1, 4)) bugsCount++;
			if (grid[index].isBug(0, 3)) bugsCount++;

		} else if (y == 4 && x == 4) {
			if (bkpGrid.isBug(2, 3))     bugsCount++;
			if (bkpGrid.isBug(3, 2))     bugsCount++;
			if (grid[index].isBug(3, 4)) bugsCount++;
			if (grid[index].isBug(4, 3)) bugsCount++;

		} else if (y == 0) {
			if (bkpGrid.isBug(2, 1))             bugsCount++;
			if (grid[index].isBug(x - 1, y    )) bugsCount++;
			if (grid[index].isBug(x + 1, y    )) bugsCount++;
			if (grid[index].isBug(x    , y + 1)) bugsCount++;

		} else if (y == 4) {
			if (bkpGrid.isBug(2, 3))             bugsCount++;
			if (grid[index].isBug(x - 1, y    )) bugsCount++;
			if (grid[index].isBug(x + 1, y    )) bugsCount++;
			if (grid[index].isBug(x    , y - 1)) bugsCount++;

		} else if (x == 0) {
			if (bkpGrid.isBug(1, 2))             bugsCount++;
			if (grid[index].isBug(x    , y - 1)) bugsCount++;
			if (grid[index].isBug(x    , y + 1)) bugsCount++;
			if (grid[index].isBug(x + 1, y    )) bugsCount++;

		} else if (x == 4) {
			if (bkpGrid.isBug(3, 2))             bugsCount++;
			if (grid[index].isBug(x    , y - 1)) bugsCount++;
			if (grid[index].isBug(x    , y + 1)) bugsCount++;
			if (grid[index].isBug(x - 1, y    )) bugsCount++;

		} else if (y == 1 && x == 2) {
			for (int i = 0; i < Tile::W; i++) {
				if (grid[index + 1].isBug(i, 0)) bugsCount++;
			}
			if (grid[index].isBug(x - 1, y    )) bugsCount++;
			if (grid[index].isBug(x + 1, y    )) bugsCount++;
			if (grid[index].isBug(x    , y - 1)) bugsCount++;

		} else if (y == 2 && x == 1) {
			for (int i = 0; i < Tile::H; i++) {
				if (grid[index + 1].isBug(0, i)) bugsCount++;
			}
			if (grid[index].isBug(x    , y - 1)) bugsCount++;
			if (grid[index].isBug(x    , y + 1)) bugsCount++;
			if (grid[index].isBug(x - 1, y    )) bugsCount++;

		} else if (y == 2 && x == 3 ) {
			for( int i = 0; i < 5; i++  ) {
				if (grid[index + 1].isBug(4, i)) bugsCount++;
			}
			if (grid[index].isBug(x    , y - 1)) bugsCount++;
			if (grid[index].isBug(x    , y + 1)) bugsCount++;
			if (grid[index].isBug(x + 1, y    )) bugsCount++;

		} else if (y == 3 && x == 2) {
			for (int i = 0; i < 5; i++) {
				if (grid[index + 1].isBug(i, 4)) bugsCount++;
			}
			if (grid[index].isBug(x - 1, y    )) bugsCount++;
			if (grid[index].isBug(x + 1, y    )) bugsCount++;
			if (grid[index].isBug(x    , y + 1)) bugsCount++;

		} else {
			if (grid[index].isBug(x    , y - 1)) bugsCount++;
			if (grid[index].isBug(x    , y + 1)) bugsCount++;
			if (grid[index].isBug(x - 1, y    )) bugsCount++;
			if (grid[index].isBug(x + 1, y    )) bugsCount++;
		}
	}

	return bugsCount;
}


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
		int startLayer = 200;

		Tile grid[GRID_SIZE];

		grid[startLayer] = map;

		for (int i = 0; i < 200; i++) {
			Tile bkpGrid;

			for (int j = 0; j < 403; j++) {
				Tile newGrid = grid[j];

				for (int y = 0; y < Tile::H; y++) {
					for (int x = 0; x < Tile::W; x++) {
						int bugsCount = getBugsCount(grid, bkpGrid, j, x, y);

						if (grid[j].isBug(x, y) && bugsCount != 1) {
							newGrid.clear(x, y);

						} else if (! grid[j].isBug(x, y) && (bugsCount == 1 || bugsCount == 2)) {
							newGrid.set(x, y);
						}
					}
				}

				bkpGrid = grid[j];

				grid[j] = newGrid;
			}
		}

		int bugs = 0;
		for (auto &g : grid) {
			for (int y = 0; y < Tile::H; y++) {
				for (int x = 0; x < Tile::W; x++) {
					bugs += g.isBug(x, y);
				}
			}
		}

		PRINTF(("PART_B: %d", bugs));
	}
}
