#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int64_t countMatches(
	const std::string          &str,
	const std::vector<int>     &blocks,
	std::map<int64_t, int64_t> &cache,
	uint16_t                    strPos,
	uint16_t                    blockPos,
	uint16_t                    blockLength
) {
	int64_t key = ((int64_t) strPos << 32) | ((int64_t) blockPos << 16) | (blockLength);

	{
		auto it = cache.find(key);

		if (it != cache.end()) {
			return it->second;
		}
	}

	if (strPos == str.size()) {
		if ((blockPos == blocks.size()) && (blockLength == 0)) {
			return 1;

		} else if (
			(blockPos == (blocks.size() - 1)) &&
			(blocks[blockPos] == blockLength)
		) {
			return 1;

		} else {
			return 0;
		}
	}

	{
		int64_t ret = 0;

		for (char c : { '.', '#' }) {
			if (str[strPos] == c || str[strPos] == '?') {
				if (c == '.') {
					if (blockLength == 0) {
						ret += countMatches(str, blocks, cache, strPos + 1, blockPos, 0);

					} else if (blockPos < blocks.size() && blocks[blockPos] == blockLength) {
						ret += countMatches(str, blocks, cache, strPos + 1, blockPos + 1, 0);
					}

				} else if (c == '#') {
					ret += countMatches(str, blocks, cache, strPos + 1, blockPos, blockLength + 1);
				}
			}
		}

		cache[key] = ret;

		return ret;
	}
}

static int64_t solve(const std::string &str, const std::vector<int> &groups, int repetitions) {
	std::string strToTest = str;
	std::vector<int> gropusToTest = groups;

	for (int i = 1; i < repetitions; i++) {
		strToTest += ("?" + str);

		gropusToTest.insert(gropusToTest.begin(), groups.begin(), groups.end());
	}

	{
		std::map<int64_t, int64_t> cache;

		return countMatches(strToTest, gropusToTest, cache, 0, 0, 0);
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int64_t partA = 0;
	int64_t partB = 0;

	for (auto &l : lines) {
		auto toks = utils::strTok(l, ' ');

		auto record   = toks[0];
		auto criteria = toks[1];

		std::vector<int> criteriaNumbers;

		{
			auto nums = utils::strTok(criteria, ',');

			for (auto &num : nums) {
				criteriaNumbers.push_back(std::stoi(num));
			}
		}

		partA += solve(record, criteriaNumbers, 1);
		partB += solve(record, criteriaNumbers, 5);
	}

	PRINTF(("PART_A: %" PRId64, partA));
	PRINTF(("PART_B: %" PRId64, partB));
}
