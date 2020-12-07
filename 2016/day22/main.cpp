#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Node {
	int size;
	int used;
	int avail;

	Point<int> pos;

	Node() {
		this->size  = 0;
		this->used  = 0;
		this->avail = 0;
	}

	Node(int size, int used, int avail, int x, int y) {
		this->pos   = Point<int>(x, y);
		this->size  = size;
		this->used  = used;
		this->avail = avail;
	}
};


struct Map {
	Map() {
		this->nodes = nullptr;
		this->maxX  = 0;
		this->maxY  = 0;
	}

	void init(const std::vector<Node *> &nodes) {
		for (auto node : nodes) {
			if (node->pos.x() > this->maxX) {
				this->maxX = node->pos.x();
			}

			if (node->pos.y() > this->maxY) {
				this->maxY = node->pos.y();
			}
		}

		this->maxX++;
		this->maxY++;

		this->nodes = new Node**[this->maxY];

		for (int y = 0; y < this->maxY; y++) {
			this->nodes[y] = new Node*[this->maxX];
		}
		for (auto node : nodes) {
			this->nodes[node->pos.y()][node->pos.x()] = node;
		}
	}

	void dump() const {
		for (int y = 0; y < this->maxY; y++) {
			printf("|");
			for (int x = 0; x < this->maxX; x++) {
				Node *node = this->nodes[y][x];

				if (node->used == 0) {
					printf("_");
				} else if (node->avail == 0) {
					printf("#");
				} else {
					printf(".");
				}
			}
			printf("|\n");
		}
		printf("\n");
	}

	Node ***nodes;
	int     maxX;
	int     maxY;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<Node *> nodes;

	int maxX = 0;
	int maxY = 0;

	for (auto line : lines) {
		int x, y, size, used, avail;

		if (sscanf(line.c_str(), "/dev/grid/node-x%d-y%d %dT %dT %dT %*d%%", &x, &y, &size, &used, &avail) != 5) {
			continue;
		}

		nodes.push_back(new Node(size, used, avail, x, y));

		if (x > maxX) {
			maxX = x;
		}

		if (y > maxY) {
			maxY = y;
		}
	}

	{
		std::vector<std::vector<int>> combinations;

		utils::genCombinations(combinations, nodes.size(), 2);

		int validPairs = 0;

		for (auto &comb : combinations) {
			Node *nodeA = nodes[comb[0] - 1];
			Node *nodeB = nodes[comb[1] - 1];

			bool validPair = false;

			if (nodeA->used > 0) {
				if (nodeA->used <= nodeB->avail) {
					validPair = true;
				}
			}

			if (! validPair) {
				if (nodeB->used > 0) {
					if (nodeB->used <= nodeA->avail) {
						validPair = true;
					}
				}
			}

			if (validPair) {
				validPairs++;
			}
		}

		PRINTF(("PART_A: %d", validPairs));
	}

	{
		Map map;

		map.init(nodes);
		map.dump();
	}
}
