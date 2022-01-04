#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int solve(const std::vector<int> &lines, bool partA) {
	int ret = 0;

	{
		auto jumpOffsets = lines;

		int position = 0;

		do {
			int offset      = jumpOffsets[position];
			int newPosition = position + offset;

			if (partA) {
				jumpOffsets[position]++;

			} else {
				if (offset >= 3) {
					jumpOffsets[position]--;

				} else {
					jumpOffsets[position]++;
				}
			}

			position = newPosition;

			ret++;
		} while (position >= 0 && position < jumpOffsets.size());
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = utils::toIntV(File::readAllLines(argv[1]));

	PRINTF(("PART_A: %d", solve(lines, true)));
	PRINTF(("PART_B: %d", solve(lines, false)));

	return 0;
}
