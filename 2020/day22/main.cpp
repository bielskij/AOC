#include <assert.h>
#include <queue>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Player {
	std::list<int> deck;
	int id;

	Player() {
		this->id = -1;
	}

	void reset() {
		this->deck.clear();

		this->id = -1;
	}

	bool isNonSet() const {
		return this->id == -1;
	}

	bool hasCards() const {
		return ! this->deck.empty();
	}

	int getResult() {
		int sum = 0;

		{
			int factor = this->deck.size();

			for (auto v : this->deck) {
				sum += (v * factor);

				factor--;
			}
		}

		return sum;
	}

	void cropDeck(int size) {
		while (this->deck.size() > size) {
			this->deck.pop_back();
		}
	}
};


bool game(Player *playerA, Player *playerB, int depth, bool partA) {
	int loops = 0;

	while (playerA->hasCards() && playerB->hasCards()) {
		int cardA = playerA->deck.front(); playerA->deck.pop_front();
		int cardB = playerB->deck.front(); playerB->deck.pop_front();

		bool winnerA = true;

		if (partA) {
			if (cardA < cardB) {
				winnerA = false;
			}

		} else {
			if (cardA <= playerA->deck.size() && cardB <= playerB->deck.size()) {
				Player pA = *playerA;
				Player pB = *playerB;

				pA.cropDeck(cardA);
				pB.cropDeck(cardB);

				winnerA = game(&pA, &pB, depth + 1, partA);

			} else if (cardA < cardB) {
				winnerA = false;
			}
		}

		if (winnerA) {
			playerA->deck.push_back(cardA);
			playerA->deck.push_back(cardB);

		} else {
			playerB->deck.push_back(cardB);
			playerB->deck.push_back(cardA);
		}

		loops++;
		if (loops > 2000) {
			return true;
		}
	}

	if (playerA->deck.empty()) {
		return false;
	}

	return true;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Player> players;

		{
			Player current;

			for (auto &l : lines) {
				if (l.empty()) {
					if (! current.isNonSet()) {
						players.push_back(current);

						current.reset();
					}

				} else {
					if (l.compare(0, 6, "Player") == 0) {
						sscanf(l.c_str(), "Player %d", &current.id);

					} else {
						current.deck.push_back(utils::toInt(l));
					}
				}
			}

			if (! current.isNonSet()) {
				players.push_back(current);
			}
		}

		{
			std::vector<Player> p = players;

			PRINTF(("PART_A: %d", (game(&p[0], &p[1], 0, true) ? p[0] : p[1]).getResult()));
		}

		{
			std::vector<Player> p = players;

			PRINTF(("PART_B: %d", (game(&p[0], &p[1], 0, false) ? p[0] : p[1]).getResult()));
		}
	}

	return 0;
}
