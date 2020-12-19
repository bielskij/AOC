#define _GLIBCXX_REGEX_STATE_LIMIT 1000000
#include <regex>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


std::string getRegex(std::map<int, std::string> &rules, int ruleid, bool partB) {
	auto rule = rules[ruleid];

	if (partB) {
		switch (ruleid) {
			case 8:
				{
					return "(" + getRegex(rules, 42, partB) + "+)";
				}
				break;

			case 11:
				{
					std::string str = "(";

					for (int i = 2; i < 10; ++i) {
						str += (
							getRegex(rules, 42, partB) + "{" + utils::toString(i) + "}" +
							getRegex(rules, 31, partB) + "{" + utils::toString(i) + "}|"
						);
					}

					return str + getRegex(rules, 42, partB) + getRegex(rules, 31, partB) + ")";
				}
				break;
		}
	}

	if (rule[0] == '"') { // letter
		return rule.substr(1, 1);

	} else {
		std::string regexPart = "(";

		auto toks = utils::strTok(rule, ' ');
		for (auto &tok : toks) {
			if (tok == "|") {
				regexPart.push_back('|');

			} else if (utils::isNumber(tok)) {
				regexPart += getRegex(rules, atoi(tok.c_str()), partB);
			}
		}

		regexPart += ")";

		return regexPart;
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<int, std::string> rules;
		std::vector<std::string>   data;

		{
			bool parsingRules = true;

			for (auto &line : lines) {
				if (line.empty()) {
					parsingRules = false;
					continue;
				}

				if (parsingRules) {
					auto toks = utils::strTok(line, ':');

					rules[atoi(toks[0].c_str())] = utils::trim(toks[1], " ");

				} else {
					data.push_back(line);
				}
			}
		}

		{
			int partA = 0;

			std::regex regex(getRegex(rules, 0, false));

			for (auto &line : data) {
				if (std::regex_match(line.c_str(), regex)) {
					partA++;
				}
			}

			PRINTF(("PART_A: %d", partA));
		}

		{
			int partB = 0;

			std::regex regex(getRegex(rules, 0, true));

			for (auto &line : data) {
				if (std::regex_match(line.c_str(), regex)) {
					partB++;
				}
			}

			PRINTF(("PART_B: %d", partB));
		}
	}

	return 0;
}
