#include <functional>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


enum class Direction {
	FIRST,

	R = FIRST,
	L,
	U,
	D,

	LAST
};

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		Point<int> start;

		for (int y = 0; y < lines.size(); y++) {
			auto &row = lines[y];

			int x = row.find_first_of('S', 0);
			if (x >= 0) {
				start.set(x, y);
				break;
			}
		}

		// right, left, up, down
		std::vector<Point<int>> directions = {
			{ 1, 0 }, { -1, 0 }, { 0, -1 }, { 0, 1 }
		};

		std::vector<Direction> startDirections;

		std::map<Direction, std::map<char, Direction>> transitions = {
			{ Direction::R, {
				{ '-', Direction::R },
				{ '7', Direction::D },
				{ 'J', Direction::U }
			}},

			{ Direction::L, {
				{ '-', Direction::L },
				{ 'F', Direction::D },
				{ 'L', Direction::U },
			}},

			{ Direction::U, {
				{ '|', Direction::U },
				{ 'F', Direction::R },
				{ '7', Direction::L },
			}},

			{ Direction::D, {
				{ '|', Direction::D },
				{ 'L', Direction::R },
				{ 'J', Direction::L },
			}},
		};

		{
			Direction i = Direction::FIRST;

			while (i != Direction::LAST) {
				auto direction = directions[(int)i];

				int nx = start.x() + direction.x();
				int ny = start.y() + direction.y();

				if (
					(nx >= 0 && nx < lines[0].length()) &&
					(ny >= 0 && ny < lines.size())
				) {
					char c = lines[ny][nx];

					if (transitions[i].find(c) != transitions[i].end()) {
						startDirections.push_back(i);
					}
				}

				i = (Direction)((int)i + 1);
			}
		}

		std::map<int, std::map<int, int>> visited;
		Direction  currDir = startDirections[0];
		Point<int> currPos;

		currPos.x(start.x() + directions[(int)currDir].x());
		currPos.y(start.y() + directions[(int)currDir].y());

		visited[start.x()][start.y()]++;

		int pathLength = 1;

		while (start != currPos) {
			visited[currPos.x()][currPos.y()]++;

			pathLength++;

			currDir = transitions[currDir][lines[currPos.y()][currPos.x()]];

			currPos.x(currPos.x() + directions[(int)currDir].x());
			currPos.y(currPos.y() + directions[(int)currDir].y());
		};

		PRINTF(("PART_A: %d", pathLength / 2));

		{
			int enclosedTiles = 0;

			for (int y = 0; y < lines.size(); y++) {
				auto &line = lines[y];

				bool withinLoop = false;

				for (int x = 0; x < line.length(); x++) {
					if (visited[x][y]) {
						char c = lines[y][x];

						if (c == '|' || c == 'J' || c == 'L') {
							withinLoop = ! withinLoop;
						}

					} else if (withinLoop) {
						enclosedTiles++;
					}
				}
			}

			PRINTF(("PART_B: %d", enclosedTiles));
		}
	}

	return 0;
}
