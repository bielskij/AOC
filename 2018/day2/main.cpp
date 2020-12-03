#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	{
		int twoTimeLetters   = 0;
		int threeTimeLetters = 0;

		for (auto &id : data) {
			std::map<char, int> letters;

			bool twoIncreased   = false;
			bool threeIncreased = false;

			for (auto c : id) {
				if (letters[c] == 0) {
					letters[c] = std::count(id.begin(), id.end(), c);

					if (letters[c] == 2) {
						if (! twoIncreased) {
							twoTimeLetters++;

							twoIncreased = true;
						}

					} else if (letters[c] == 3) {
						if (! threeIncreased) {
							threeTimeLetters++;

							threeIncreased = true;
						}
					}
				}
			}
		}

		PRINTF(("PART_A: %d", twoTimeLetters * threeTimeLetters));
	}

	{
		for (auto &src : data) {
			for (auto &candidate : data) {
				if (src == candidate) {
					continue;
				}

				int differPosition = -1;
				for (int idx = 0; idx < src.length(); idx++) {
					if (src[idx] != candidate[idx]) {
						if (differPosition >= 0) {
							break;
						}

						differPosition = idx;
					}

					if (idx == src.length() - 1) {
						std::string result = src.substr(0, differPosition) + src.substr(differPosition + 1);

						PRINTF(("PART_B: %s", result.c_str()));

						return 0;
					}
				}
			}
		}
	}

	return 0;
}
