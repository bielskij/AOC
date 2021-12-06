#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<int> lanterfishes = utils::toIntV(utils::strTok(lines[0], ','));

		for (int i = 0; i < 80; i++) {
			int lcount = lanterfishes.size();

			for (int lidx = 0; lidx < lcount; lidx++) {
				int &age = lanterfishes[lidx];

				if (age == 0) {
					age = 6;

					lanterfishes.push_back(8);

				} else {
					age--;
				}
			}
		}

		PRINTF(("PART_A: %zd", lanterfishes.size()));
	}

	{
		int64_t countOfAge[9] = { 0 };

		for (auto l : utils::toIntV(utils::strTok(lines[0], ','))) {
			countOfAge[l] += 1;
		}

		for (int i = 0; i < 256; i++) {
			int64_t tmp = countOfAge[0];

			for (int j = 0; j < 8; j++) {
				countOfAge[j] = countOfAge[j + 1];
			}

			countOfAge[8]  = tmp;
			countOfAge[6] += tmp;
		}

		int64_t result = 0;

		for (int i = 0; i < 9; i++) {
			result += countOfAge[i];
		}

		PRINTF(("PART_B: %ld", result));
	}

	return 0;
}
