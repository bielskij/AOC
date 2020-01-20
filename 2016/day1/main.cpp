#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchDijkstra.h"

#include "common/debug.h"


static int getId(const Point<int> &p) {
	int ret = 0;

	ret += std::abs(p.x());
	ret += std::abs(p.y() * 1000);

	if (p.x() < 0) {
		ret += 1000000;
	}

	if (p.y() < 0) {
		ret += 10000000;
	}

	return ret;
}


int main(int argc, char *argv[]) {
	std::vector<std::string> data = utils::strTok(File::readAllLines(argv[1])[0], ' ');

	{
		Point<int> partAPosition;

		char currentDirection = 'N';
		for (auto i : data) {
			char direction;
			int  length;

			if (sscanf(i.c_str(), "%c%d", &direction, &length) != 2) {
				abort();
			}

			switch (currentDirection) {
				case 'N': currentDirection = (direction == 'R') ? 'E' : 'W'; break;
				case 'E': currentDirection = (direction == 'R') ? 'S' : 'N'; break;
				case 'S': currentDirection = (direction == 'R') ? 'W' : 'E'; break;
				case 'W': currentDirection = (direction == 'R') ? 'N' : 'S'; break;

				default:
					abort();
			}

			Point<int> lastPosition = partAPosition;

			switch (currentDirection) {
				case 'N': partAPosition.y(partAPosition.y() + length); break;
				case 'E': partAPosition.x(partAPosition.x() + length); break;
				case 'S': partAPosition.y(partAPosition.y() - length); break;
				case 'W': partAPosition.x(partAPosition.x() - length); break;
			}
		}

		PRINTF(("PART_A: %d", utils::manhattanDistance<int>(Point<int>(0, 0), partAPosition)));
	}
}
