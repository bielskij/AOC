#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<std::pair<int64_t, int64_t>> ranges;
		std::vector<std::pair<int64_t, int64_t>> mergedRanges;
		
		std::vector<int64_t> ingredients;

		for (auto &l : lines) {
			if (l.empty()) {
				continue;
			}

			auto borders = utils::strTok(l, '-');

			if (borders.size() == 1) {
				ingredients.push_back(std::stoll(borders[0]));

			} else {
				ranges.push_back({ std::stoll(borders[0]), std::stoll(borders[1]) });
			}
		}

		{
			std::sort(ranges.begin(), ranges.end(), [](const std::pair<int64_t, int64_t> &a, const std::pair<int64_t, int64_t> &b) {
				return a.first < b.first;
			});

			auto current = ranges[0];

			for (size_t i = 1; i < ranges.size(); i++) {
				auto &next = ranges[i];

				if (next.first <= current.second + 1) {
					current.second = std::max(current.second, next.second);
					
				} else {
					mergedRanges.push_back(current);

					current = next;
				}
			}

			mergedRanges.push_back(current);
		}

		{
			int partA = 0;

			for (auto ingredient : ingredients) {
				for (const auto &r : ranges) {
					if (ingredient >= r.first && ingredient <= r.second) {
						partA++;
						break;
					}
				}
			}

			PRINTF(("PART_A: %d", partA));
		}

		{
			long long int partB = 0;

			for (auto &m : mergedRanges) {
				partB += ((m.second - m.first) + 1);
			}

			PRINTF(("PART_B: %lld", partB));
		}
	}

	return 0;
}
