#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int64_t solve(const std::vector<std::string> &stones, int iterations) {
	int64_t ret = 0;

	{
		std::map<std::string, int64_t> stoneMap;

		for (const auto &stone : stones) {
			stoneMap[stone] = 1;
		}

		while (iterations--) {
			std::map<std::string, int64_t> newStoneMap;

			for (const auto &stoneP : stoneMap) {
				const auto &stone = stoneP.first;
				auto count = stoneP.second;

				long long val = std::stoll(stone);

				if (val == 0) {
					newStoneMap["1"] += count;

				} else if (stone.length() & 1) {
					newStoneMap[std::to_string(val * 2024)] += count;

				} else {
					long long a = std::stoll(stone.substr(0, stone.length() / 2));
					long long b = std::stoll(stone.substr(stone.length() / 2));

					newStoneMap[std::to_string(a)] += count;
					newStoneMap[std::to_string(b)] += count;
				}
			}

			stoneMap = newStoneMap;
		}

			for (const auto &stone : stoneMap) {
				ret += stone.second;
			}
	}

	return ret;
}

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<std::string> stones = utils::strTok(lines[0], ' ');

		PRINTF(("PART_A: %ld", solve(stones, 25)));
		PRINTF(("PART_B: %ld", solve(stones, 75)));
	}

	return 0;
}
