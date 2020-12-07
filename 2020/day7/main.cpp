#include <stack>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchDfs.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Bag {
	Bag (const std::string &name) : name (name) {

	}

	std::string name;

	std::vector<std::pair<Bag *, int>> iners;
	std::vector<Bag *>                 outers;
};


void getPartA(Bag *bag, std::set<Bag *> &bags) {
	bags.insert(bag);

	for (auto outer : bag->outers) {
		getPartA(outer, bags);
	}
}


int getPartB(Bag *bag) {
	int ret = 1;

	for (auto inner : bag->iners) {
		ret += (inner.second * getPartB(inner.first));
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<std::string, Bag *> bags;

	for (auto &l : lines) {
		std::string outer;
		std::vector<std::pair<std::string, int>> inners;

		auto words  = utils::strTok(l, ' ');
		auto wordIt = words.begin();

		while (wordIt != words.end()) {
			if (*wordIt == "bags") {
				wordIt++;
				break;
			}

			outer += *wordIt++;
		}

		// skip 'contain' word
		wordIt++;
		if (*wordIt == "no") {
			continue;
		}

		while (wordIt != words.end()) {
			int count = utils::toInt(*wordIt++);
			std::string inner;

			while (wordIt != words.end()) {
				if (wordIt->compare(0, 3, "bag") == 0) {
					wordIt++;
					break;
				}

				inner += *wordIt++;
			}

			inners.push_back(std::pair<std::string, int>(inner, count));
		}

		if (bags[outer] == nullptr) {
			bags[outer] = new Bag(outer);
		}

		for (auto &i : inners) {
			if (bags[i.first] == nullptr) {
				bags[i.first] = new Bag(i.first);
			}

			bags[outer]->iners.push_back(std::pair<Bag *, int>(bags[i.first], i.second));

			bags[i.first]->outers.push_back(bags[outer]);
		}
	}

	{
		std::set<Bag *> result;

		getPartA(bags["shinygold"], result);

		PRINTF(("PART_A: %zd", result.size() - 1));
	}

	{
		PRINTF(("PART_B: %d", getPartB(bags["shinygold"]) - 1));
	}

	return 0;
}
