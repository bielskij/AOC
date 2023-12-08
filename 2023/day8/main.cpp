#include <functional>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int solve(std::map<std::string, std::pair<std::string, std::string>> &transitions, const std::string &steps, const std::string &start, std::function<bool(const std::string &)> checkEnd) {
	auto it = transitions.find(start);
	int  stepIdx = 0;
	int partA = 0;

	while (! checkEnd(it->first)) {
		if (steps[stepIdx] == 'L') {
			it = transitions.find(it->second.first);

		} else {
			it = transitions.find(it->second.second);
		}

		partA++;
		stepIdx = (stepIdx + 1) % steps.size();
	}

	return partA;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::string steps;

	std::map<std::string, std::pair<std::string, std::string>> nodes;

	for (const auto &l : lines) {
		auto line = utils::trim(l, " ");

		if (line.empty()) {
			continue;
		}

		if (steps.empty()) {
			steps = line;

		} else {
			char left[16];
			char right[16];
			char node[16];

			sscanf(line.c_str(), "%[^=]= (%[^,], %[^)]", node, left, right);

			std::string nodeStr = utils::trim(node, " ");
			nodes[nodeStr] = std::pair<std::string, std::string>(utils::trim(left, " "), utils::trim(right, " "));
		}
	}

	PRINTF(("PART_A: %d", solve(nodes, steps, "AAA", [](const std::string &v) {
		return v == "ZZZ";
	})));

	{
		std::vector<int64_t> pathLength;

		for (const auto &node : nodes) {
			if (*node.first.rbegin() == 'A') {
				pathLength.push_back(solve(nodes, steps, node.first, [](const std::string &v) {
					return *v.rbegin() == 'Z';
				}));
			}
		}

		PRINTF(("PART_B: %" PRId64, utils::nww<int64_t>(pathLength)));
	}


	return 0;
}
