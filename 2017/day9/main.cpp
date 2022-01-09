#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int  depth = 0;
		bool garbage = false;
		bool escape  = false;

		int partA = 0;
		int partB = 0;

		for (auto c : lines[0]) {
			switch (c) {
				case '{':
					if (! garbage) {
						depth++;

						partA += depth;

					} else {
						if (! escape) {
							partB++;
						}

						escape = false;
					}
					break;

				case '}':
					if (! garbage) {
						depth--;

					} else {
						if (! escape) {
							partB++;
						}

						escape = false;
					}
					break;

				case '<':
					if (! garbage) {
						garbage = true;

					} else {
						if (! escape) {
							partB++;

						} else {
							escape = false;
						}
					}
					break;

				case '>':
					if (garbage) {
						if (! escape) {
							garbage = false;

						} else {
							escape = false;
						}
					}
					break;

				case '!':
					if (garbage) {
						escape = ! escape;
					}
					break;

				default:
					{
						if (garbage) {
							if (! escape) {
								partB++;
							}
						}

						escape = false;
					}
					break;
			}
		}

		PRINTF(("PART_A: %d", partA));
		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
