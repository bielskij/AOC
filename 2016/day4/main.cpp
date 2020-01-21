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

	for (auto &line : data) {
		std::map<char, int> letterMap;
		std::string checksum;
		int id = 0;

		Mode mode = MODE_NAME;
		for (auto c : line) {
			switch (mode) {
				case MODE_NAME:
					if (c != '-') {
						if (std::isdigit(c)) {
							id = c - '0';

							mode = MODE_ID;

						} else {
							letterMap[c]++;
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

		PRINTF(("lettermap: %zd, id: %d, chksum: '%s'", letterMap.size(), id, checksum.c_str()));

		if (letterMap.size() >= 5) {
			bool correct = true;

			std::vector<std::pair<char, int>> pairs;
			for (auto &p : letterMap) {
				pairs.push_back(p);
			}

			// Sort descending
			sort(pairs.begin(), pairs.end(), [=](std::pair<char, int>& a, std::pair<char, int>& b) {
				return a.second > b.second;
			});

			if (correct) {
				for (int i = 0; i < 5; i++) {
					if (checksum.find(pairs[i].first, 0) == std::string::npos) {
						correct = false;
						break;
					}
				}
			}

			if (correct) {
				int max = std::numeric_limits<int>::max();

				for (int i = 0; i < checksum.size(); i++) {
					char c = checksum[i];

					if (letterMap[c] < max) {
						max = letterMap[c];

					} else if (letterMap[c] == max) {
						if (checksum[i - 1] >= c) {
							correct = false;
							break;
						}

					} else {
						correct = false;
						break;
					}
				}
			}

			if (correct) {
				PRINTF(("%s - > correct", line.c_str()));

				partA += id;

			} else {
				PRINTF(("%s - > incorrect", line.c_str()));
			}
		}
	}

	// 224769 - za mała
	PRINTF(("PART_A: %d", partA));
}
