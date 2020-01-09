#include <jsoncpp/json/json.h>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int getMax(std::map<std::string, std::map<std::string, int>> &guests, const std::vector<std::vector<std::string>> &permutations) {
	int max = std::numeric_limits<int>::min();

	for (auto &p : permutations) {
		int tmp = 0;

		for (int i = 0; i < p.size(); i++) {
			tmp += guests[p.at(i)][p.at(i > 0 ? i - 1 : (p.size() - 1))];
			tmp += guests[p.at(i)][p.at(i == (p.size() - 1) ? 0 : i + 1)];
		}

		if (max < tmp) {
			max = tmp;
		}
	}

	return max;
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	std::map<std::string, std::map<std::string, int>> guests;

	for (auto line : data) {
		char src[32];
		char dst[32];
		int  happines;

		if (sscanf(line.c_str(), "%s would gain %d happiness units by sitting next to %32[^.]", src, &happines, dst) == 3) {
			guests[src][dst] += happines;

		} else if (sscanf(line.c_str(), "%s would lose %d happiness units by sitting next to %32[^.]", src, &happines, dst) == 3) {
			guests[src][dst] -= happines;

		} else {
			PRINTF(("Not supported data! '%s'", line.c_str()));

			abort();
		}
	}

	{
		std::vector<std::string> names;
		std::vector<std::vector<std::string>> permutations;

		for (auto g : guests) {
			names.push_back(g.first);
		}

		utils::genPermutation<std::string>(permutations, names);

		PRINTF(("PART_A: %d", getMax(guests, permutations)));

		names.push_back("part_b_guest");

		utils::genPermutation<std::string>(permutations, names);

		PRINTF(("PART_B: %d", getMax(guests, permutations)));
	}
}
