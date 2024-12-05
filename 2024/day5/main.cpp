#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<int, std::vector<int>> rules;

	bool parseRules = true;

	int partA = 0;

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

			bool valid = true;

			for (int i = 0; i < toks.size() - 1; i++) {
				const auto &rule = rules[toks[i]];

				auto it = std::find(rule.begin(), rule.end(), toks[i + 1]);
				if (it == rule.end()) {
					valid = false;
					break;
				}
			}

			if (valid) {
				partA += toks[toks.size() / 2];
			}
		}
	}

	PRINTF(("PART_A: %d", partA));

	return 0;
}
