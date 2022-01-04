#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		auto nums = utils::toIntV(utils::strTok(lines[0], '\t'));

		std::string state(nums.begin(), nums.end());

		std::set<std::string> states = { state };
		bool secondLoop = false;

		while (true) {
			int index = std::distance(state.begin(), std::max_element(state.begin(), state.end()));
			int banks = state[index];

			state[index] = 0;

			while (banks > 0) {
				index = (index + 1) % state.size();

				state[index]++;

				banks--;
			}

			if (! states.insert(state).second) {

				if (! secondLoop) {
					PRINTF(("PART_A: %zd", states.size()));

					states.clear();
					states.insert(state);
					secondLoop = true;

				} else {
					PRINTF(("PART_B: %zd", states.size()));

					break;
				}
			}
		}
	}

	return 0;
}
