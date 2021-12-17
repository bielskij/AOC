#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int xRange[2];
	int yRange[2];

	if (sscanf(
			lines[0].c_str(),
			"target area: x=%d..%d, y=%d..%d",
			&xRange[0], &xRange[1], &yRange[0], &yRange[1]
		) != 4
	) {
		PRINTF(("Unable to parse input!"));
		abort();
	}

	int partA = std::numeric_limits<int>::min();
	int partB = 0;

	for (int xVelocity = 1; xVelocity <= xRange[1]; xVelocity++) {
		for (int yVelocity = yRange[0]; yVelocity <= -yRange[0]; yVelocity++) {
			int vx = xVelocity;
			int vy = yVelocity;
			int my = std::numeric_limits<int>::min();

			int x = 0;
			int y = 0;

			while ((x < xRange[0] || y > yRange[1]) && (x <= xRange[1]) && (y >= yRange[0])) {
				x += vx;
				y += vy;

				if (y > my) {
					my = y;
				}

				if (vx > 0) {
					vx--;

				} else if (vx < 0) {
					vx++;
				}

				vy--;
			}

			if ((x >= xRange[0]) && (x <= xRange[1]) && (y >= yRange[0]) && (y <= yRange[1])) {
				if (my > partA) {
					partA = my;
				}

				partB++;
			}
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB));

	return 0;
}
