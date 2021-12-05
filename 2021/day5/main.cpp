#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int solve(const std::vector<Line<int>> &lines, bool partA) {
	std::map<int, std::map<int, int>> area;

	for (const auto &l : lines) {
		std::vector<Point<int>> linePoints;

		if (
			! partA ||
			(
				(l.getStart().x() == l.getEnd().x()) ||
				(l.getStart().y() == l.getEnd().y())
			)
		) {
			l.bresenham(linePoints);

			for (const auto &p : linePoints) {
				area[p.y()][p.x()]++;
			}
		}
	}

	int overlapingPoints = 0;

	for (const auto &row : area) {
		for (auto cell : row.second) {
			if (cell.second > 1) {
				overlapingPoints++;
			}
		}
	}

	return overlapingPoints;
}


int main(int argc, char *argv[]) {
	std::vector<Line<int>> lines;

	{
		auto input = File::readAllLines(argv[1]);

		for (const auto &l : input) {
			int x1, x2, y1, y2;

			if (sscanf(l.c_str(), "%d,%d -> %d,%d", &x1, &y1, &x2, &y2) != 4) {
				ERR(("Unable to parse!"));

			} else {
				lines.push_back(Line<int>(x1, y1, x2, y2));
			}
		}
	}

	PRINTF(("PART_A: %d", solve(lines, true)));
	PRINTF(("PART_B: %d", solve(lines, false)));

	return 0;
}
