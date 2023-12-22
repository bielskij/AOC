#include <queue>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/Maze.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	Point<int> src;

	for (int y = 0; y < lines.size(); y++) {
		auto &l = lines[y];

		auto xPos = l.find_first_of('S');
		if (xPos != std::string::npos) {
			src.x(xPos);
			src.y(y);
			break;
		}
	}

	{
		std::vector<Point<int>> gardens = { src };

		std::vector<Point<int>> dirMod = {
			{ 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 }
		};

		for (int step = 1; step < 65; step++) {
			std::vector<Point<int>> newGardens;

			std::map<Point<int>, bool> visited;

			for (auto &garden : gardens) {
				for (auto &mod : dirMod) {
					auto next = garden + mod;

					if (
						next.x() >= 0 && next.x() < lines[0].length() &&
						next.y() >= 0 && next.y() < lines.size() &&
						! visited[next]
					) {
						switch (lines[next.y()][next.x()]) {
							case '.':
							case 'S':
								newGardens.push_back(next);
								break;
						}

						visited[next] = true;
					}
				}
			}

			if (step == 64) {
				PRINTF(("PART_A: %zd", newGardens.size()));
			}

			gardens = newGardens;
		}
	}

	return 0;
}
