#include <map>
#include <stack>
#include <unordered_map>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static std::string solve(std::vector<std::string> &txt, bool partA) {
	std::map<int, std::string> stacks;

	for (auto &l : txt) {
		int amount, from, to;
		if (sscanf(l.c_str(), "move %d from %d to %d", &amount, &from, &to) == 3) {
			auto &fromList = stacks[from - 1];
			auto &toList = stacks[to - 1];

			{
				std::string toMove = fromList.substr(fromList.length() - amount);

				if (partA) {
					std::reverse(toMove.begin(), toMove.end());
				}

				toList.append(toMove);

				fromList = fromList.substr(0, fromList.length() - amount);
			}

		} else {
			for (size_t i = 0; i < l.length(); i += 4) {
				if (l[i] == '[' && l[i + 2] == ']') {
					auto &list = stacks[i / 4];

					list.insert(0, 1, l[i + 1]);
				}
			}
		}
	}

	{
		std::string ret(stacks.size(), ' ');

		for (auto &p : stacks) {
			ret[p.first] = *p.second.rbegin();
		}

		return ret;
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %s", solve(lines, true).c_str()));
	PRINTF(("PART_B: %s", solve(lines, false).c_str()));

	return 0;
}
