#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int checkSum = 0;

	for (auto &l : lines) {
		auto words = utils::strTok(l, '\t');

		int min = std::numeric_limits<int>::max();
		int max = std::numeric_limits<int>::min();

		for (auto &n : words) {
			int val = utils::toInt(n.c_str());

			if (val > max) {
				max = val;
			}

			if (val < min) {
				min = val;
			}
		}

		checkSum += std::abs(max - min);
	}

	PRINTF(("PART_A: %d", checkSum));

	checkSum = 0;

	for (auto &l : lines) {
		auto words = utils::strTok(l, '\t');

		int lastchecksum = checkSum;
		for (int i = 0; i < words.size(); i++) {
			int a = utils::toInt(words[i]);

			for (int j = 0; j < words.size(); j++) {
				int b;

				if (i == j) {
					continue;
				}

				b = utils::toInt(words[j]);

				if (b == 0) {
					continue;
				}

				int rest;
				int div;
				if (a > b) {
					rest = a % b;
					if (rest == 0) {
						div = a / b;
					}

				} else {
					rest = b % a;
					if (rest == 0) {
						div = b / a;
					}
				}

				if (rest == 0) {
					checkSum += div;
					break;
				}
			}

			if (lastchecksum != checkSum) {
				break;
			}
		}
	}

	PRINTF(("PART_B: %d", checkSum));
}
