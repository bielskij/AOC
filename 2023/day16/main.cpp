#include <stack>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


enum Direction {
	NORTH,
	EAST,
	SOUTH,
	WEST
};


struct Context {
	Direction  direction;
	Point<int> pos;

	Context(Direction dir, const Point<int> &p) {
		this->direction = dir;
		this->pos       = p;
	}

	Context() {
		this->direction = Direction::NORTH;
		this->pos       = { 0, 0 };
	}
};


static bool isValid(const Point<int> &p, const std::vector<std::string> &lines) {
	if (
		(p.y() < 0 || p.y() >= lines.size()) ||
		(p.x() < 0 || p.x() >= lines[0].size())
	) {
		return false;
	}

	return true;
}


static int solve(const std::vector<std::string> &lines, const Point<int> &start) {
	std::set<Direction> visited[lines.size()][lines[0].length()];

	std::stack<Context> nextPoints;

	nextPoints.push(Context(Direction::EAST, start));

	const std::array<Point<int>, 4> directionMod = {
		Point<int>(0, -1), Point<int>(1, 0), Point<int>(0, 1), Point<int>(-1, 0)
	};

	while (! nextPoints.empty()) {
		auto top = nextPoints.top(); nextPoints.pop();

		visited[top.pos.y()][top.pos.x()].insert(top.direction);

		std::vector<Context> possiblePoints;

		switch (lines[top.pos.y()][top.pos.x()]) {
			case '.':
				possiblePoints.push_back(Context(top.direction, top.pos + directionMod[top.direction]));
				break;

			case '|':
				if (top.direction == Direction::NORTH || top.direction == Direction::SOUTH) {
					possiblePoints.push_back(Context(top.direction, top.pos + directionMod[top.direction]));

				} else {
					possiblePoints.push_back(Context(Direction::NORTH, top.pos + directionMod[Direction::NORTH]));
					possiblePoints.push_back(Context(Direction::SOUTH, top.pos + directionMod[Direction::SOUTH]));
				}
				break;

			case '-':
				if (top.direction == Direction::EAST || top.direction == Direction::WEST) {
					possiblePoints.push_back(Context(top.direction, top.pos + directionMod[top.direction]));

				} else {
					possiblePoints.push_back(Context(Direction::EAST, top.pos + directionMod[Direction::EAST]));
					possiblePoints.push_back(Context(Direction::WEST, top.pos + directionMod[Direction::WEST]));
				}
				break;

			case '/':
				if (top.direction == Direction::EAST) {
					possiblePoints.push_back(Context(Direction::NORTH, top.pos + directionMod[Direction::NORTH]));

				} else if (top.direction == Direction::WEST) {
					possiblePoints.push_back(Context(Direction::SOUTH, top.pos + directionMod[Direction::SOUTH]));

				} else if (top.direction == Direction::NORTH) {
					possiblePoints.push_back(Context(Direction::EAST, top.pos + directionMod[Direction::EAST]));

				} else {
					possiblePoints.push_back(Context(Direction::WEST, top.pos + directionMod[Direction::WEST]));
				}
				break;

			case '\\':
				if (top.direction == Direction::EAST) {
					possiblePoints.push_back(Context(Direction::SOUTH, top.pos + directionMod[Direction::SOUTH]));

				} else if (top.direction == Direction::WEST) {
					possiblePoints.push_back(Context(Direction::NORTH, top.pos + directionMod[Direction::NORTH]));

				} else if (top.direction == Direction::NORTH) {
					possiblePoints.push_back(Context(Direction::WEST, top.pos + directionMod[Direction::WEST]));

				} else {
					possiblePoints.push_back(Context(Direction::EAST, top.pos + directionMod[Direction::EAST]));
				}
				break;
		}

		for (auto &c : possiblePoints) {
			auto &visitedSet = visited[c.pos.y()][c.pos.x()];

			if (isValid(c.pos, lines) && visitedSet.find(c.direction) == visitedSet.end()) {
				nextPoints.push(c);
			}
		}
	}

	{
		int result = 0;

		for (int y = 0; y < lines.size(); y++) {
			for (int x = 0; x < lines[0].length(); x++) {
				result += ! visited[y][x].empty();
			}
		}

		return result;
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		PRINTF(("PART_A: %d", solve(lines, Point<int>(0, 0))));

		{
			std::vector<Point<int>> pointToCheck;

			for (int x = 0; x < lines[0].size(); x++) {
				pointToCheck.push_back({ 0, x });
				pointToCheck.push_back({ (int)lines.size() - 1, x});
			}

			for (int y = 1; y < lines.size() - 3; y++) {
				pointToCheck.push_back({ y, 0 });
				pointToCheck.push_back({ y, (int)lines.size() - 1});
			}

			int partB = 0;

			for (auto &p : pointToCheck) {
				int res = solve(lines, p);
				if (res > partB) {
					partB = res;
				}
			}

			PRINTF(("PART_B: %d", partB));
		}
	}

	return 0;
}
