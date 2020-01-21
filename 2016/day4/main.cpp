#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


enum Mode {
	MODE_NAME,
	MODE_ID,
	MODE_CHECKSUM
};


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	int partA = 0;
	int partB = 0;

	for (auto &line : data) {
		std::map<char, int> letterMap;
		std::string encrypted;
		std::string checksum;
		int id = 0;

		Mode mode = MODE_NAME;
		for (auto c : line) {
			switch (mode) {
				case MODE_NAME:
					{
						if (! std::isdigit(c)) {
							encrypted += c;

							if (c != '-') {
								letterMap[c]++;
							}

						} else {
							id = c - '0';

							mode = MODE_ID;
						}
					}
					break;

				case MODE_ID:
					if (c != '[') {
						id *= 10;
						id += c - '0';

					} else {
						mode = MODE_CHECKSUM;
					}
					break;

				case MODE_CHECKSUM:
					if (c != ']') {
						checksum += c;
					}
					break;
			}
		}

		// Part A
		{
			bool correct = true;

			if (letterMap.size() < 5) {
				correct = false;
			}

			if (correct) {
				if (checksum.size() != 5) {
					correct = false;
				}
			}

			if (correct) {
				std::vector<std::pair<char, int>> pairs;
				for (auto &p : letterMap) {
					pairs.push_back(p);
				}

				// Sort descending
				sort(pairs.begin(), pairs.end(), [=](std::pair<char, int>& a, std::pair<char, int>& b) {
					if (a.second > b.second) {
						return true;

					} else if (a.second == b.second) {
						return a.first < b.first;
					}

					return false;
				});

				for (int i = 0; i < 5; i++) {
					if (checksum[i] != pairs[i].first) {
						correct = false;
						break;
					}
				}
			}

			if (correct) {
				partA += id;
			}
		}

		{
			int total = 'z' - 'a' + 1;

			std::string decrypted;

			for (auto c : encrypted) {
				if (c == '-') {
					decrypted += ' ';

				} else {
					decrypted.push_back((((c - 'a') + id) % total) + 'a');
				}
			}

			if (decrypted.find("northpole") != std::string::npos) {
				partB = id;
			}
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB));
}
