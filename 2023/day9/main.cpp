#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int partA = 0;
		int partB = 0;

		for (auto &l : lines) {
			std::vector<std::vector<int>> report;

			{
				std::vector<int> history;

				for (auto &num : utils::strTok(l, ' ')) {
					history.push_back(std::stoi(num));
				}

				report.push_back(history);
			}

			bool allZero = false;

			do {
				allZero = true;

				auto &last = *report.rbegin();

				for (auto n : last) {
					if (n != 0) {
						allZero = false;
					}
				}

				if (! allZero) {
					std::vector<int> prediction;

					for (int i = 1; i < last.size(); i++) {
						prediction.push_back(last[i] - last[i - 1]);
					}

					report.push_back(prediction);
				}
			} while (! allZero);

			{
				int curr = 0;
				for (int i = report.size() - 2; i >= 0; i--) {
					int prev = *report[i].rbegin();

					curr = curr + prev;
				}

				partA += curr;
			}

			{
				int curr = 0;

				for (int i = report.size() - 2; i >= 0; i--) {
					int prev = *report[i].begin();

					curr = prev - curr;
				}

				partB += curr;
			}
		}

		PRINTF(("PART_A: %d", partA));
		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
