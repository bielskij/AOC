#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int checkOrder(const std::map<int, std::vector<int>> &rules, const std::vector<int> &pages) {
	for (int i = 0; i < pages.size() - 1; i++) {
		auto ruleIt = rules.find(pages[i]);
		if (ruleIt == rules.end()) {
			return i;
		}

		if (std::find(ruleIt->second.begin(), ruleIt->second.end(), pages[i + 1]) == ruleIt->second.end()) {
			return i + 1;
		}
	}

	return -1;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<int, std::vector<int>> rules;

	bool parseRules = true;

	int partA = 0;
	int partB = 0;

	for (const auto &l : lines) {
		if (l.empty()) {
			parseRules = false;

			continue;
		}

		if (parseRules) {
			auto toks = utils::toIntV<int>(utils::strTok(l, '|'));

			rules[toks[0]].push_back(toks[1]);

		} else {
			auto toks = utils::toIntV<int>(utils::strTok(l, ','));

			auto checkRet = checkOrder(rules, toks);
			if (checkRet < 0) {
				partA += toks[toks.size() / 2];

			} else {
				std::sort(toks.begin(), toks.end(), [&rules](int l, int r) {
					auto ruleIt = rules.find(r);
					if (ruleIt != rules.end()) {
						if (std::find(ruleIt->second.begin(), ruleIt->second.end(), l) != ruleIt->second.end()) {
							return true;
						}
					}

					return false;
				});

				partB += toks[toks.size() / 2];
			}
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB));

	return 0;
}
