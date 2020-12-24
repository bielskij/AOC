#include <climits>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

enum Color {
	WHITE,
	BLACK
};


struct Floor {
	Floor() {
		this->minX = INT_MAX;
		this->maxX = INT_MIN;
		this->minY = INT_MAX;
		this->maxY = INT_MIN;
	}

	Color getColor(const Point<int> &p) {
		Color ret = WHITE;

		auto row = this->floor.find(p.y());
		if (row != this->floor.end()) {
			auto col = row->second.find(p.x());
			if (col != row->second.end()) {
				ret = col->second;
			}
		}

		return ret;
	}

	void setColor(const Point<int> &p, Color color) {
		if (p.x() > this->maxX) this->maxX = p.x();
		if (p.x() < this->minX) this->minX = p.x();
		if (p.y() > this->maxY) this->maxY = p.y();
		if (p.y() < this->minY) this->minY = p.y();

		this->floor[p.y()][p.x()] = color;
	}

	void spread() {
		this->minX--;
		this->maxX++;
		this->minY--;
		this->maxY++;
	}

	void toggle(const Point<int> &p) {
		this->setColor(p, (this->getColor(p) == WHITE) ? BLACK : WHITE);
	}

	int getBlack() {
		int ret = 0;

		for (auto row : this->floor) {
			for (auto col : row.second) {
				if (col.second == BLACK) {
					ret++;
				}
			}
		}

		return ret;
	}

	std::map<int, std::map<int, Color>> floor;

	int minX, maxX, minY, maxY;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	Floor floor;

	for (const auto &line : lines) {
		Point<int> center;

		for (int i = 0; i < line.size(); i++) {
			switch (line[i]) {
				// e, se, sw, w, nw, ne
				case 'n':
					{
						center.incY();

						if (line[i + 1] == 'e') {
							center.incX();
						}

						i++;
					}
					break;

				case 'e':
					center.incX();
					break;

				case 's':
					{
						center.decY();

						if (line[i + 1] == 'w') {
							center.decX();
						}

						i++;
					}
					break;

				case 'w':
					center.decX();
					break;
			}
		}

		floor.toggle(center);
	}

	PRINTF(("PART_A: %d", floor.getBlack()));

	for (int day = 0; day < 100; day++) {
		floor.spread();

		Floor oldFloor = floor;

		for (int x = oldFloor.minX; x <= oldFloor.maxX; x++) {
			for (int y = oldFloor.minY; y <= oldFloor.maxY; y++) {
				int neighbours = 0;

				if (oldFloor.getColor(Point<int>(x,     y + 1)) == BLACK) neighbours++;
				if (oldFloor.getColor(Point<int>(x + 1, y + 1)) == BLACK) neighbours++;
				if (oldFloor.getColor(Point<int>(x + 1, y    )) == BLACK) neighbours++;
				if (oldFloor.getColor(Point<int>(x,     y - 1)) == BLACK) neighbours++;
				if (oldFloor.getColor(Point<int>(x - 1, y - 1)) == BLACK) neighbours++;
				if (oldFloor.getColor(Point<int>(x - 1, y    )) == BLACK) neighbours++;

				if (oldFloor.getColor(Point<int>(x, y)) == BLACK) {
					if (neighbours == 0 || neighbours > 2) {
						floor.setColor(Point<int>(x, y), WHITE);
					}

				} else {
					if (neighbours == 2) {
						floor.setColor(Point<int>(x, y), BLACK);
					}
				}
			}
		}
	}

	PRINTF(("PART_B: %d", floor.getBlack()));

	return 0;
}
