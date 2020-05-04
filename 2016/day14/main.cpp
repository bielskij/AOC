#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::string salt = argv[1];

	std::vector<std::pair<char, int>> tripplets;
	std::map<char, std::vector<int>>  fifthlets;

	int counter = 0;
	bool endloop = false;

	while (! endloop) {
		std::string hash;

		for (int i = 0; i < 1000; i++) {
			bool threeFound = false;

			hash = utils::md5(salt + utils::toString(counter), true);

			for (int j = 0; j < hash.size() - 2; j++) {
				char c = hash[j];
				int  increase = 0;

				bool isCorrect = true;

				// Minimum three times in a row
				if (isCorrect) {
					if (c != hash[j + 1] || c != hash[j + 2]) {
						isCorrect = false;
					}
				}

				// A letter before
				if (isCorrect) {
					if ((j > 0) && (hash[j - 1] == c)) {
						isCorrect = false;
					}
				}

				// Check for 3/5
				if (isCorrect) {
					bool hasFifth = false;

					if (j < (hash.size() - 4)) {
						// Five or more
						if ((c == hash[j + 3]) && (c == hash[j + 4])) {
							hasFifth = true;
						}
					}

					if (hasFifth) {
						fifthlets[c].push_back(counter);

						increase = 5;

					}

					{
						bool hasThird = ! threeFound;

						if (hasThird) {
							tripplets.push_back(std::pair<char, int>(c, counter));
							threeFound = true;

							increase = 3;
						}
					}

					if (increase != 0) {
						j += (increase - 1);
					}
				}
			}

			counter++;
		}

		{
			int keyCount = 0;

			for (auto &t : tripplets) {
				int fifthlethsInNextTousend = 0;

				for (auto f : fifthlets[t.first]) {
					if (t.second >= f) {
						continue;
					}

					if (f - t.second <= 1000) {
						fifthlethsInNextTousend++;
					}
				}

				if (fifthlethsInNextTousend != 0) {
					keyCount++;

					if (keyCount == 64) {
						PRINTF(("PART_A: %d", t.second));
						endloop = true;
					}
				}

				if (endloop) {
					break;
				}
			}
		}
	}
}
