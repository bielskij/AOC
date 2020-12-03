#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int getTrees(const std::vector<std::string> &map, int rightSteps, int downSteps) {
	int trees = 0;

	Point<int> position;

	while (position.y() < map.size()) {
		switch (map[position.y()][position.x() % map[0].length()]) {
			case '#':
				trees++;
				break;

			default:
				break;
		}

		position.x(position.x() + rightSteps);
		position.y(position.y() + downSteps);
	}

	return trees;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", getTrees(lines, 3, 1)));

	{
		long long int partB = 1;

		partB *= getTrees(lines, 1, 1);
		partB *= getTrees(lines, 3, 1);
		partB *= getTrees(lines, 5, 1);
		partB *= getTrees(lines, 7, 1);
		partB *= getTrees(lines, 1, 2);

		PRINTF(("PART_B: %lld", partB));
	}
}
