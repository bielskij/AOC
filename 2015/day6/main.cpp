#include <stdexcept>
#include <climits>
#include <set>
#include <algorithm>

#include <openssl/md5.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


#define WIDTH  1000
#define HEIGHT 1000


struct Rule {
	enum Action {
		ACTION_ON,
		ACTION_OFF,
		ACTION_TOGGLE
	};

	Rule(const std::string &str) {
		auto words = utils::strTok(str, ' ');

		int idx = 0;

		if (words[idx].compare("turn") == 0) {
			++idx;

			if (words[idx].compare("on") == 0) {
				this->action = ACTION_ON;
			} else {
				this->action = ACTION_OFF;
			}
			++idx;

		} else if (words[idx].compare("toggle") == 0) {
			this->action = ACTION_TOGGLE;

			++idx;

		} else {
			ERR(("Not supported word! (%s)", words[idx].c_str()));

			abort();
		}

		{
			auto coords = utils::strTok(words[idx], ',');

			this->start = Point(utils::toInt(coords[0]), utils::toInt(coords[1]));

			idx += 2;

			coords = utils::strTok(words[idx], ',');

			this->end = Point(utils::toInt(coords[0]), utils::toInt(coords[1]));
		}

	}

	Action action;
	Point  start;
	Point  end;
};


int main(int argc, char *argv[]) {
	std::vector<Rule> rules;

	{
		std::vector<std::string> textRules = File::readAllLines(argv[1]);

		for (auto textRule = textRules.begin(); textRule != textRules.end(); textRule++) {
			rules.push_back(Rule(*textRule));
		}
	}

	{
		char grid[HEIGHT][WIDTH] = {};

		for (auto rule = rules.begin(); rule != rules.end(); rule++) {
			for (int y = rule->start.getY(); y <= rule->end.getY(); y++) {
				for (int x = rule->start.getX(); x <= rule->end.getX(); x++) {
					switch (rule->action) {
						case Rule::ACTION_ON:     grid[y][x]  = 1; break;
						case Rule::ACTION_OFF:    grid[y][x]  = 0; break;
						case Rule::ACTION_TOGGLE: grid[y][x] ^= 1; break;
					}
				}
			}
		}

		int sum = 0;
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				sum += grid[y][x];
			}
		}

		PRINTF(("PART_A: %d", sum));
	}

	{
			char grid[HEIGHT][WIDTH] = {};

			for (auto rule = rules.begin(); rule != rules.end(); rule++) {
				for (int y = rule->start.getY(); y <= rule->end.getY(); y++) {
					for (int x = rule->start.getX(); x <= rule->end.getX(); x++) {
						switch (rule->action) {
							case Rule::ACTION_ON:     grid[y][x] += 1; break;
							case Rule::ACTION_OFF:    grid[y][x] -= 1; break;
							case Rule::ACTION_TOGGLE: grid[y][x] += 2; break;
						}

						if (grid[y][x] < 0) {
							grid[y][x] = 0;
						}
					}
				}
			}

			int sum = 0;
			for (int y = 0; y < HEIGHT; y++) {
				for (int x = 0; x < WIDTH; x++) {
					sum += grid[y][x];
				}
			}

			PRINTF(("PART_B: %d", sum));
		}


//
//	for (int y = 0; y < HEIGHT; y++) {
//		for (int x = 0; x < WIDTH; x++) {
//			grid[y][x] = 1;
//		}
//	}
//
//	for (int x = 0; x < WIDTH; x++) {
//		grid[0][x] ^= 1;
//	}
//
//	for (int y = 499; y <= 500; y++) {
//		for (int x = 499; x <= 500; x++) {
//			grid[y][x] = 0;
//		}
//	}
//

}
