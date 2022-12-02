#include <functional>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

#define LOSE 0
#define DRAW 1
#define WIN  2

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int scoreA = 0;
	int scoreB = 0;

	std::function<int(char, char)> getScore = [](char o, char m) {
		int ret = 0;

		if (m == o) {
			ret += (3 + m);

		} else {
			ret += m;

			if (m == ((o + 1) % 3)) {
				ret += 6;
			}
		}

		return ret + 1;
	};

	for (auto &line : lines) {
		char opponent = line[0] - 'A';
		char my       = line[2] - 'X';

		scoreA += getScore(opponent, my);

		switch (my) {
			case LOSE:
				my = (opponent + 2) % 3;
				break;

			case DRAW:
				my = opponent;
				break;

			case WIN:
				my = (opponent + 1) % 3;
				break;
		}

		scoreB += getScore(opponent, my);
	}

	PRINTF(("PART_A: %d", scoreA));
	PRINTF(("PART_B: %d", scoreB));

	return 0;
}
