#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Dice {
	int lastVal;
	int rolledCount;

	Dice() {
		this->lastVal     = 0;
		this->rolledCount = 0;
	}

	int next() {
		int ret = this->lastVal + 1;

		this->lastVal = (this->lastVal + 1) % 100;

		if (this->lastVal == 0) {
			this->rolledCount++;
		}

		return ret;
	}
};


struct Player {
	int     position;
	int64_t score;
	int     id;

	Player() {
		this->position = 1;
		this->score    = 0;
		this->id       = 0;
	}
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Player> players;

		for (const auto &l : lines) {
			Player p;

			if (sscanf(l.c_str(), "Player %d starting position: %d", &p.id, &p.position) == 2) {
				players.push_back(p);

			} else {
				abort();
			}
		}

		{
			Dice dice;
			bool gameOver = false;

			while (! gameOver) {
				for (auto &player : players) {
					for (int j = 0; j < 3; j++) {
						int diceVal = dice.next();

						player.position = 1 + (((player.position - 1) + diceVal) % 10);
					}

					player.score += player.position;
					if (player.score >= 1000) {
						gameOver = true;
						break;
					}
				}
			}

			for (const auto &p : players) {
//				PRINTF(("Player %d has %d points", p.id, p.score));

				if (p.score < 1000) {
					int64_t partA = dice.rolledCount * 100 + dice.lastVal;
					PRINTF(("PART_A: %ld", partA * p.score));
				}
			}
		}
	}

	return 0;
}
