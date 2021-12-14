#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int64_t solve(const std::string &polymer, const std::map<std::string, char> &rules, int steps) {
	std::map<std::string, int64_t> pairs;
	std::map<char, int64_t> elements;

	{
		int idx = 0;

		while (idx < polymer.length() - 1) {
			pairs[polymer.substr(idx, 2)]++;

			idx += 1;
		}

		for (char p : polymer) {
			elements[p]++;
		}
	}

	for (int step = 0; step < steps; step++) {
		auto newPairs = pairs;

		for (const auto &p : pairs) {
			auto it = rules.find(p.first);

			std::string replacements[2] = {
				{ p.first[0], it->second },
				{ it->second, p.first[1] }
			};

			int64_t n = pairs[p.first];

			newPairs[p.first]    -= n;

			elements[p.first[0]] -= n;
			elements[p.first[1]] -= n;

			newPairs[replacements[0]] += n;
			newPairs[replacements[1]] += n;

			elements[replacements[0][0]] += n;
			elements[replacements[0][1]] += n;
			elements[replacements[1][1]] += n;
		}

		pairs = newPairs;
	}

	auto max = std::max_element(elements.begin(), elements.end(), [](const std::pair<char, int64_t> &p1, const std::pair<char, int64_t> &p2) {
		return p1.second < p2.second;
	});

	auto min = std::max_element(elements.begin(), elements.end(), [](const std::pair<char, int64_t> &p1, const std::pair<char, int64_t> &p2) {
		return p1.second > p2.second;
	});

	return max->second - min->second;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<std::string, char> rules;
		std::string polymer;

		for (const auto &l : lines) {
			char pattern[3] = { 0 };
			char insertion;

			if (l.empty()) {
				continue;
			}

			if (sscanf(l.c_str(), "%02s -> %c", pattern, &insertion) == 2) {
				rules[pattern] = insertion;

			} else {
				polymer = l;
			}
		}
#if 0
		{
			std::string p = polymer;

			for (int step = 0; step < 10; step++) {
				int idx = 0;

				p.reserve(p.length() + p.length() - 1);

				while (idx < p.length() - 1) {
					char insertion = rules[p.substr(idx, 2)];

					p.insert(idx + 1, 1, insertion);

					idx += 2;
				}
			}

			{
				std::map<char, int64_t> quantities;

				for (auto c : p) {
					quantities[c]++;
				}

				auto max = std::max_element(quantities.begin(), quantities.end(), [](const std::pair<char, int64_t> &p1, const std::pair<char, int64_t> &p2) {
					return p1.second < p2.second;
				});

				auto min = std::max_element(quantities.begin(), quantities.end(), [](const std::pair<char, int64_t> &p1, const std::pair<char, int64_t> &p2) {
					return p1.second > p2.second;
				});

				PRINTF(("PART_A: %d (%ld, %ld)", max->second - min->second, min->second, max->second));
			}
		}
#endif

		PRINTF(("PART_A: %ld", solve(polymer, rules, 10)));
		PRINTF(("PART_B: %ld", solve(polymer, rules, 40)));
	}

	return 0;
}
