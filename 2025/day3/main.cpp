#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int partA = 0;

		for (const auto &line : lines) {
			int max = 0;

			for (size_t startIdx = 0; startIdx < line.length() - 1; startIdx++) {
				for (size_t nextIdx = startIdx + 1; nextIdx < line.length(); nextIdx++) {
					int number = (line[startIdx] - '0') * 10 + (line[nextIdx] - '0');

					if (number > max) {
						max = number;
					}
				}
			}

			partA += max;
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		long long int partB = 0;

		for (const auto &line : lines) {
			std::vector<int> positions;

			while (positions.size() < 12) {
				int off = positions.empty() ? 0 : *positions.rbegin() + 1;

				int maxVal = line[off];
				int maxOff = off++;

				for (; off < line.length() - (12 - positions.size() - 1); off++) {
					if (line[off] > maxVal) {
						maxVal = line[off];
						maxOff = off;
					}
				}

				positions.push_back(maxOff);
			}

			{
				std::string str;

				str.reserve(12);

				for (auto pos : positions) {
					str.push_back(line[pos]);
				}

				partB += std::stoll(str);
			}
		}

		PRINTF(("PART_B: %lld", partB));
	}

	return 0;
}
