#include <array>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static const std::array<int, 3> modsX = { -1, 0, 1 };
static const std::array<int, 3> modsY = { -1, 0, 1 };

static int removePapers(const std::vector<std::string> &src, std::vector<std::string> *dst) {
	int ret = 0;

	if (dst) {
		*dst = src;
	}

	for (int y = 0; y < src.size(); y++) {
		const auto &l = src[y];

		for (int x = 0; x < l.length(); x++) {
			int papers = 0;

			if (l[x] != '@') {
				continue;
			}

			for (auto modX : modsX) {
				for (auto modY : modsY) {
					Point<int> toCheck(x + modX, y + modY);

					if (modX == 0 && modY == 0) {
						continue;
					}

					if (
						toCheck.x() < 0 ||
						toCheck.x() >= l.length() ||
						toCheck.y() < 0 ||
						toCheck.y() >= src.size()
					) {
						continue;
					}

					if (src[toCheck.y()][toCheck.x()] == '@') {
						papers++;
					}
				}
			}

			if (papers < 4) {
				if (dst) {
					(*dst)[y][x] = '.';
				}

				ret++;
			}
		}
	}

	return ret;
}

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);


	PRINTF(("PART_A: %d", removePapers(lines, nullptr)));

	{
		int partB = 0;

		auto src = lines;
		int removed;
		do {
			auto dst = src;

			removed = removePapers(src, &dst);

			src = dst;

			partB += removed;
		} while (removed > 0);

		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
