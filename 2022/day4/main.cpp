#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int partA = 0;
	int partB = 0;

	for (auto &l : lines) {
		std::string a;
		std::string b;

		{
			int aStart, aEnd, bStart, bEnd;

			if (sscanf(l.c_str(), "%d-%d,%d-%d", &aStart, &aEnd, &bStart, &bEnd) == 4) {
				for (int i = aStart; i <= aEnd; i++) {
					a.push_back(i + '0');
				}

				for (int i = bStart; i <= bEnd; i++) {
					b.push_back(i + '0');
				}

				if (a.find(b) != std::string::npos) {
					partA++;

				} else if (b.find(a) != std::string::npos) {
					partA++;
				}

				for (auto c : a) {
					if (b.find(c) != std::string::npos) {
						partB++;
						break;
					}
				}
			}
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB));

	return 0;
}
