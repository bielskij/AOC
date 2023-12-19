#include <queue>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Rule {
	Rule(std::string &part, int value, char _operator, const std::string &workflow) : workflow(workflow), part(part) {
		this->operand   = value;
		this->_operator = _operator;
	}

	Rule(const std::string &workflow) : workflow(workflow) {
		this->operand = 0;
		this->_operator = 0;
	}

	bool eval(int value) const {
		bool ret = true;

		switch (this->_operator) {
			case '<': ret = value < this->operand; break;
			case '>': ret = value > this->operand; break;
			case 0:
				break;

			default:
				abort();
				ret = false;
		}

		return ret;
	}

	char        _operator;
	int         operand;
	std::string workflow;
	std::string part;
};


struct Workflow {
	std::vector<Rule> rules;
};


struct Ranges {
	Ranges(const std::string &workflow) : workflow(workflow) {
		this->categories["x"] = { 1, 4000 };
		this->categories["m"] = { 1, 4000 };
		this->categories["s"] = { 1, 4000 };
		this->categories["a"] = { 1, 4000 };
	}

	int64_t getCombinations() const {
		int64_t ret = 1;

		for (auto &c : categories) {
			ret *= (c.second.second - c.second.first + 1);
		}

		return ret;
	}

	bool hasCombinations() const {
		for (auto &c : categories) {
			if (c.second.second < c.second.first) {
				return false;
			}
		}

		return true;
	}

	std::string workflow;

	std::map<std::string, std::pair<int, int>> categories;
};


static int solve(const std::map<std::string, Workflow> &workflows, const std::map<std::string, int> &rating) {
	int ret = 0;

	std::string part = "in";

	auto it = workflows.find(part);
	do {
		for (auto &rule : it->second.rules) {
			if (rule.part.empty()) {
				part = rule.workflow;
				break;

			} else {
				auto valIt = rating.find(rule.part);
				if (valIt == rating.end()) {
					ERR(("No rating for part '%s'", rule.part.c_str()));
					abort();

				} else {
					if (rule.eval(valIt->second)) {
						part = rule.workflow;
						break;
					}
				}
			}
		}

		it = workflows.find(part);
	} while (it != workflows.end());

	if (part == "A") {
		for (auto &r : rating) {
			ret += r.second;
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		bool parsingWorkflows = true;

		std::map<std::string, Workflow> workflows;
		std::vector<std::map<std::string, int>> ratings;

		for (auto &l : lines) {
			if (l.empty()) {
				parsingWorkflows = false;
				continue;
			}

			if (parsingWorkflows) {
				int sepPosition = l.find('{');

				std::string name  = l.substr(0, sepPosition);
				std::string rules = l.substr(0, l.length() - 1).substr(sepPosition + 1);

				auto &workFlow = workflows[name];

				for (auto &rule : utils::strTok(rules, ',')) {
					auto index = rule.find_first_of('<');
					if (index != std::string::npos) {
						auto toks  = utils::strTok(rule, ':');
						auto toks2 = utils::strTok(toks[0], '<');

						workFlow.rules.push_back(Rule(toks2[0], std::stoi(toks2[1]), '<', toks[1]));

					} else {
						index = rule.find_first_of('>');
						if (index != std::string::npos) {
							auto toks  = utils::strTok(rule, ':');
							auto toks2 = utils::strTok(toks[0], '>');

							workFlow.rules.push_back(Rule(toks2[0], std::stoi(toks2[1]), '>', toks[1]));

						} else {
							workFlow.rules.push_back(Rule(rule));
						}
					}
				}

			} else {
				std::map<std::string, int> rating;

				for (auto &r : utils::strTok(l.substr(1, l.length() - 2), ',')) {
					auto pair = utils::strTok(r, '=');

					rating[pair[0]] = std::stoi(pair[1]);
				}

				ratings.push_back(std::move(rating));
			}
		}

		{
			int64_t partA = 0;

			for (auto &rating : ratings) {
				partA += solve(workflows, rating);
			}

			PRINTF(("PART_A: %" PRId64, partA));
		}

		{
			int64_t partB = 0;

			std::queue<Ranges> queue;

			queue.push(Ranges("in"));

			while (! queue.empty()) {
				Ranges current = queue.front(); queue.pop();

				if (! current.hasCombinations()) {
					continue;
				}

				if (current.workflow == "A") {
					partB += current.getCombinations();

					continue;
				}

				const auto &workflow = workflows[current.workflow];
				for (auto &rule : workflow.rules) {
					Ranges newRange = current;

					newRange.workflow = rule.workflow;

					if (rule.part.empty()) {
						queue.push(newRange);

					} else {
						if (rule._operator == '<') {
							newRange.categories[rule.part].second = rule.operand - 1;

							current.categories[rule.part].first = rule.operand;

						} else {
							newRange.categories[rule.part].first = rule.operand + 1;

							current.categories[rule.part].second = rule.operand;
						}

						queue.push(newRange);
					}
				}
			}

			PRINTF(("PART_B: %" PRId64, partB));
		}
	}

	return 0;
}
