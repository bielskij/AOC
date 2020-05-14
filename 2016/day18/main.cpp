#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static bool isTrap(char c) {
	return c == '^';
}


static int safeCount(const std::string &str) {
	return std::count(str.begin(), str.end(), '.');
}


int main(int argc, char *argv[]) {
	std::string src   = argv[1];
	int         loops = utils::toInt(argv[2]) - 1;

	{
		int partA = safeCount(src);

		std::string last = src;

		for (int i = 0; i < loops; i++) {
			bool traps[3]{ true };

			std::string newRow;

			for (int p = 0; p < last.length(); p++) {
				if (p == 0) {
					traps[0] = false;

				} else {
					traps[0] = isTrap(last[p - 1]);
				}

				traps[1] = isTrap(last[p]);

				if (p == last.length() - 1) {
					traps[2] = false;

				} else {
					traps[2] = isTrap(last[p + 1]);
				}

				if (
					(traps[0] && traps[1] && ! traps[2]) ||
					(! traps[0] && traps[1] && traps[2]) ||
					(traps[0] && ! traps[1] && ! traps[2]) ||
					(! traps[0] && ! traps[1] && traps[2])
				) {
					newRow.push_back('^');

				} else {
					newRow.push_back('.');
				}
			}

//			PRINTF(("%s", newRow.c_str()));

			last = newRow;

			partA += safeCount(last);
		}

		PRINTF(("RESULT: %d", partA));
	}
}
