#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int pos = 50;

	int partA  = 0;
	int partB  = 0;

	char direction = 'R';

	for (auto &rule : lines) {
		int stepDir = rule[0];
		int steps   = atoi(rule.substr(1).c_str());

		int loops   = steps / 100;
		int rest    = steps % 100;

		if (direction != stepDir) {
			pos = (100 - pos) % 100;

			direction = stepDir;
		}

		int newPos = pos + rest;

		partB += loops;
		if (rest != 0 && pos != 0 && (newPos >= 100)) {
			partB++;
		}
		
		newPos = newPos % 100;
		if (newPos == 0) {
			partA++;
		}

		pos = newPos;
	}

	PRINTF(("PART A: %d, PART B: %d", partA, partB));

	return 0;
}
