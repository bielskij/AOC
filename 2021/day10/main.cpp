#include <stack>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<char, char> m;
	std::map<char, int> scoreAMap;
	std::map<char, int> scoreBMap;

	m['('] = ')';
	m['{'] = '}';
	m['['] = ']';
	m['<'] = '>';

	scoreAMap[']'] = 57;
	scoreAMap['}'] = 1197;
	scoreAMap[')'] = 3;
	scoreAMap['>'] = 25137;

	scoreBMap['('] = 1;
	scoreBMap['['] = 2;
	scoreBMap['{'] = 3;
	scoreBMap['<'] = 4;

	int partA = 0;
	std::vector<int64_t> partB;

	for (const auto &line : lines) {
		std::stack<char> stack;
		char invalidChar = 0;

		for (auto c : line) {
			if (m.find(c) != m.end()) {
				stack.push(c);

			} else {
				if (c != m[stack.top()]) {
					invalidChar = c;
					break;

				} else {
					stack.pop();
				}
			}
		}

		if (invalidChar == 0) {
			int64_t score = 0;

			while (! stack.empty()) {
				score = score * 5 + scoreBMap[stack.top()];

				stack.pop();
			}

			partB.push_back(score);

		} else {
			partA += scoreAMap[invalidChar];
		}
	}

	PRINTF(("PART_A: %d", partA));

	std::sort(partB.begin(), partB.end());

	PRINTF(("PART_B: %ld", partB[partB.size() / 2]));

	return 0;
}
