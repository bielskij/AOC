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
		Point<int> partBPosition;

		std::vector<int> visited;

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

			if (partBPosition.x() == 0 && partBPosition.y() == 0) {
				if (partAPosition.x() != lastPosition.x()) {
					for (int x = lastPosition.x(); x != partAPosition.x(); x += (partAPosition.x() > lastPosition.x()) ? 1 : -1) {
						Point<int> tmp(x, partAPosition.y());

						if (std::find(visited.begin(), visited.end(), getId(tmp)) != visited.end()) {
							partBPosition = tmp;

						} else {
							visited.push_back(getId(tmp));
						}
					}

				} else {
					for (int y = lastPosition.y(); y != partAPosition.y(); y += (partAPosition.y() > lastPosition.y()) ? 1 : -1) {
						Point<int> tmp(partAPosition.x(), y);

						if (std::find(visited.begin(), visited.end(), getId(tmp)) != visited.end()) {
							partBPosition = tmp;

						} else {
							visited.push_back(getId(tmp));
						}
					}
				}
			}
		}

		PRINTF(("PART_A: %d", utils::manhattanDistance<int>(Point<int>(0, 0), partAPosition)));
		PRINTF(("PART_B: %d", utils::manhattanDistance<int>(Point<int>(0, 0), partBPosition)));
	}
}
