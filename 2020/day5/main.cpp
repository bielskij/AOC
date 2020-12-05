#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int partA = 0;

	bool seats[128][8] = { false };

	for (auto &line : lines) {
		int rowMin = 0;
		int rowMax = 127;
		int colMin = 0;
		int colMax = 7;
		int id;

		for (auto l : line) {
			int rowStep = (rowMax - rowMin + 1) / 2;
			int colStep = (colMax - colMin + 1) / 2;

			switch (l) {
				case 'F':
					rowMax -= rowStep;
					break;

				case 'B':
					rowMin += rowStep;
					break;

				case 'R':
					colMin += colStep;
					break;

				case 'L':
					colMax -= colStep;
					break;

				default:
					break;
			}
		}

		id = 8 * rowMin + colMax;

		if (id > partA) {
			partA = id;
		}

		seats[rowMax][colMin] = true;

		DBG(("Row %d:%d, Col: %d:%d, Id: %d", rowMin, rowMax, colMin, colMax, id));
	}
#if 0
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 128; i++) {
			printf("%c", seats[i][j] ? 'x' : ' ');
		}
		printf("\n");
	}
	printf("\n");
#endif

	PRINTF(("PART_A: %d", partA));

	for (int j = 1; j < 7; j++) {
		for (int i = 0; i < 128; i++) {
			if (! seats[i][j]) {
				if (seats[i][j - 1] && seats[i][j + 1]) {
					PRINTF(("PART_B: %d", i * 8 + j));
				}
			}
		}
	}
}
