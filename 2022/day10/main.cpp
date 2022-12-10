#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


void onPixel(char crt[6][40], int cycle, int x) {
	int pixelY = (cycle - 1) / 40;
	int pixelX = (cycle - 1) % 40;

	if (x >= pixelX - 1 && x < pixelX + 2 ) {
		crt[pixelY][pixelX] = '#';

	} else {
		crt[pixelY][pixelX] = '.';
	}
}


int onCycle(char crt[6][40], int cycle, int x) {
	int ret = 0;

	switch (cycle) {
		case 20:  ret = cycle * x; break;
		case 60:  ret = cycle * x; break;
		case 100: ret = cycle * x; break;
		case 140: ret = cycle * x; break;
		case 180: ret = cycle * x; break;
		case 220: ret = cycle * x; break;
	}

	return ret;
}


void onCheck(char crt[6][40], int cycle, int partA) {
	if (cycle == 220) {
		PRINTF(("PART_A: %d", partA));

	} else if (cycle == 240) {
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 40; x++) {
				printf("%c", crt[y][x]);
			}
			printf("\n");
		}

		exit(1);
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int x = 1;
		int cycle = 0;

		int partA = 0;

		char crt[6][40];


		for (auto &l : lines) {
			cycle++;

			onPixel(crt, cycle, x);
			partA += onCycle(crt, cycle, x);
			onCheck(crt, cycle, partA);

			if (l == "noop") {
				continue;
			}

			cycle++;

			onPixel(crt, cycle, x);
			partA += onCycle(crt, cycle, x);
			onCheck(crt, cycle, partA);

			{
				auto toks = utils::strTok(l, ' ');

				x += utils::toInt(toks[1]);
			}
		}
	}

	return 0;
}
