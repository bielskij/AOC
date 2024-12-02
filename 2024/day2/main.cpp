#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static bool _isValid(const std::vector<int64_t> &toks) {
	bool valid = true;

	if (toks[0] == toks[1]) {
		valid = false;

	} else {
		bool asc = (toks[0] < toks[1]) ? true : false;

		for (int i = 0; i < toks.size() - 1; i++) {
			int diff = toks[i + 1] - toks[i];

			if ((std::abs(diff) < 1) || (std::abs(diff) > 3)) {
				valid = false;
				break;
			}

			if ((asc && (diff < 0)) || (! asc && (diff > 0))) {
				valid = false;
				break;
			} 
		}
	}

	return valid;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int partA = 0;
	int partB = 0;

	for (const auto &line : lines) {
		auto toks = utils::toInt64tV(utils::strTok(line, ' '));

		if (_isValid(toks)) {
			partA++;
			partB++;

		} else {
			auto it = toks.begin();

			while (it != toks.end()) {
				std::vector<int64_t> tmp;

				std::copy(toks.begin(), it, std::back_inserter(tmp));
				std::copy(it + 1, toks.end(), std::back_inserter(tmp));

				if (_isValid(tmp)) {
					partB++;
					break;
				}

				++it;
			}
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB));

	return 0;
}
