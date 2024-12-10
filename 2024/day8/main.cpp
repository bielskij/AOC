#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<char, std::vector<Point<int>>> antennas;

		Point<int> size(lines[0].length(), lines.size());

		for (int y = 0; y < lines.size(); y++) {
			const auto &row = lines[y];

			for (int x = 0; x < row.length(); x++) {
				auto c = row[x];

				if (c != '.') {
					antennas[c].push_back(Point<int>(x, y));
				}
			}
		}

		std::set<Point<int>> antinodes;

		for (const auto &antennaCoords : antennas) {
			const auto &coords = antennaCoords.second;

			for (int src = 0; src < coords.size(); src++) {
				for (int dst = 0; dst < coords.size(); dst++) {
					auto &srcPoint = coords[src];
					auto &dstPoint = coords[dst];

					if (src == dst) {
						continue;
					}

					Point<int> v(
						(srcPoint.x() - dstPoint.x()), 
						(srcPoint.y() - dstPoint.y())
					);

					Point<int> ov = srcPoint + v;

					if (
						ov.x() >= 0 && ov.y() >= 0 &&
						ov.x() < size.x() && ov.y() < size.y()
					) {
						antinodes.insert(ov);
					}
				}
			}
		}

		PRINTF(("PART_A: %zd", antinodes.size()));
	}

	return 0;
}
