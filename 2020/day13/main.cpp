#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int timestamp = utils::toInt(lines[0]);

		std::vector<std::pair<int, int>> ids;

		{
			auto toks = utils::strTok(lines[1], ',');
			int  idx  = 0;

			for (auto &tok : toks) {
				if (tok != "x") {
					ids.push_back(std::pair<int, int>(idx, utils::toInt(tok)));
				}

				idx++;
			}
		}

		{
			int result = 0;
			for (int ts = timestamp; result == 0; ts++) {
				for (auto id : ids) {
					if ((ts % id.second) == 0) {
						result = id.second * (ts - timestamp);
						break;
					}
				}
			}

			PRINTF(("PART_A: %d", result));
		}

		{
			long long int t  = 0;
			long long int dt = 1;

			for (auto id : ids) {
				while (1) {
					if (((id.first + t) % id.second) == 0) {
						break;
					}

					t += dt;
				}

				dt *= id.second;
			}

			PRINTF(("PART_B: %lld", t));
		}
	}

	return 0;
}
