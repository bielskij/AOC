#include <climits>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::map<std::string, std::map<std::string, int>> locations;

	{
		auto lines = File::readAllLines(argv[1]);

		for (auto line = lines.begin(); line != lines.end(); line++) {
			auto tokens = utils::strTok(*line, ' ');

			locations[tokens[0]][tokens[2]] = utils::toInt(tokens[4]);
			locations[tokens[2]][tokens[0]] = utils::toInt(tokens[4]);
		}
	}

	{
		std::vector<std::vector<std::string>> combinations;

		{
			std::vector<std::string> keys;

			for (auto p = locations.begin(); p != locations.end(); p++) {
				keys.push_back(p->first);
			}

			utils::genPermutation<std::string>(combinations, keys);

			PRINTF(("Nodes: %zd, %zd", locations.size(), combinations.size()));
		}

		int min = INT_MAX;
		int max = INT_MIN;
		for (auto c = combinations.begin(); c != combinations.end(); c++) {
			int sum = 0;

			int visited = 0;
			while (visited != locations.size() - 1) {
				auto src = locations.find(c->at(visited));
				auto dst = src->second.find(c->at(visited + 1));

				if (dst != src->second.end()) {
					sum += dst->second;
					visited++;

				} else {
					break;
				}
			}

			if (visited == (locations.size() - 1)) {
				if (min > sum) {
					min = sum;
				}
				if (max < sum) {
					max = sum;
				}
			}
		}

		PRINTF(("PART_A: %d", min));
		PRINTF(("PART_B: %d", max));
	}
}
