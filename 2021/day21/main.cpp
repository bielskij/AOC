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

	int next(int turns) {
		int ret = 0;

		while (turns-- > 0) {
			ret += this->next();
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

	void handleDiceResult(int result) {
		this->position = 1 + (((this->position - 1) + result) % 10);
		this->score   += this->position;
	}
};


static const int probability[7] = {
	1, 3, 6, 7, 6, 3, 1
};


static void solvePartB(int positions[2], int scores[2], bool isP1, int64_t wins[2], int64_t occurs) {
	if (scores[0] >= 21) {
		wins[0] += occurs;

	} else if (scores[1] >= 21) {
		wins[1] += occurs;

	} else {
		int newPositions[2] = { positions[0], positions[1] };
		int newScores[2]    = { scores[0],    scores[1] };

		for (int comb = 0; comb < 7; comb++) {
			int idx = isP1 ? 0 : 1;

			newPositions[idx] = 1 + (((positions[idx] - 1) + (comb + 3)) % 10);
			newScores[idx]    = scores[idx] + newPositions[idx];

			solvePartB(newPositions, newScores, ! isP1, wins, probability[comb] * occurs);
		}
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Player> _players;

		for (const auto &l : lines) {
			Player p;

			if (sscanf(l.c_str(), "Player %d starting position: %d", &p.id, &p.position) == 2) {
				_players.push_back(p);

			} else {
				abort();
			}
		}

		{
			std::vector<Player> players = _players;

			Dice dice;
			bool gameOver = false;

			while (! gameOver) {
				for (auto &player : players) {
					player.handleDiceResult(dice.next(3));

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

		{
			int     positions[2] = { _players[0].position, _players[1].position };
			int64_t wins[2]      = { 0, 0};
			int     scores[2]    = { 0, 0 };

			solvePartB(positions, scores, true, wins, 1);

			PRINTF(("PART_B: %ld", wins[0] > wins[1] ? wins[0] : wins[1]));
		}
	}

	return 0;
}
