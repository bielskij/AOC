#include <stdexcept>
#include <string.h>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


#define W 50
#define H  6


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	char screen[H][W];

	memset(screen, 0, sizeof(screen));

	for (auto &l : data) {
		int a, b;

		if (sscanf(l.c_str(), "rotate column x=%d by %d", &a, &b) == 2) {
			for (int i = 0; i < b; i++) {
				char cp = screen[H - 1][a];

				for (int y = H - 1; y > 0; y--) {
					screen[y][a] = screen[y - 1][a];
				}

				screen[0][a] = cp;
			}

		} else if (sscanf(l.c_str(), "rotate row y=%d by %d", &a, &b) == 2) {
			for (int i = 0; i < b; i++) {
				char cp = screen[a][W - 1];

				for (int x = W - 1; x > 0; x--) {
					screen[a][x] = screen[a][x - 1];
				}

				screen[a][0] = cp;
			}

		} else if (sscanf(l.c_str(), "rect %dx%d", &a, &b) == 2) {
			for (int y = 0; y < b; y++) {
				for (int x = 0; x < a; x++) {
					screen[y][x] = 1;
				}
			}

		} else {
			abort();
		}
	}

	int partA = 0;

	PRINTF(("PART_B:"));

	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			printf("%c", screen[y][x] == 1 ? '#' : ' ');

			partA += screen[y][x];
		}
		printf("\n");
	}

	PRINTF(("PART_A: %d", partA));
}
