#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static const std::vector<Point<int>> directions = {
	{  0, -1 },
	{ -1,  0 },
	{  0,  1 },
	{  1,  0 }
};

static void walk(const std::vector<std::string> &lines, std::set<Point<int>> &visited, Point<int> pos, bool partA, int &sum) {
	for (const auto &mod : directions) {
		auto next = pos;

		next.x(next.x() + mod.x());
		next.y(next.y() + mod.y());

		if (next.y() >= 0 && next.y() < lines.size()) {
			const auto &row = lines[next.y()];

			if (next.x() >= 0 && next.x() < row.length()) {

				auto c = row[next.x()];

				if (c - lines[pos.y()][pos.x()] == 1) {
					switch (c) {
						case '9':
							{
								bool inc = ! partA || (visited.find(next) == visited.end());

								visited.insert(next);

								if (inc) {
									sum += 1;
								}
							}
							break;

						case '.':
							break;

						default:
							walk(lines, visited, next, partA, sum);
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int partA = 0;
	int partB = 0;

	for (int y = 0; y < lines.size(); y++) {
		const auto &row = lines[y];

		for (int x = 0; x < row.length(); x++) {
			auto c = row[x];

			if (c == '0') {
				std::set<Point<int>> visited;

				visited.clear();
				walk(lines, visited, Point<int>(x, y), true, partA);

				visited.clear();
				walk(lines, visited, Point<int>(x, y), false, partB);
			}
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB));

	return 0;
}
