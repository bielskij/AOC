#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto points = utils::toIntV(File::readAllLines(argv[1]));

	{
		int last = -1;
		int partA = 0;

		for (auto s : points) {
			if (last != -1) {
				if (s > last) {
					partA++;
				}
			}

			last = s;
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		int partB = 0;
		int last  = -1;

		for (size_t idx = 2; idx < points.size(); idx++) {
			int sum = points[idx - 2] + points[idx - 1] + points[idx];

			if (last != -1) {
				if (sum > last) {
					partB++;
				}
			}

			last = sum;
		}

		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
