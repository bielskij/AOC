#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int partA = 0;
		int partB = 0;

		std::map<int, std::pair<int, std::set<int>>> scratchCards;
		std::map<int, std::set<int>> results;

		for (const auto &l : lines) {
			auto numbers = utils::strTok(utils::strTok(l, ':')[1], '|');
			int cardId;

			sscanf(l.c_str(), "Card %d:", &cardId);

			if (scratchCards.find(cardId) == scratchCards.end()) {
				scratchCards[cardId].first = 1;
			}

			for (auto &num : utils::strTok(numbers[0], ' ')) {
				num = utils::trim(num, " ");

				if (! num.empty()) {
					scratchCards[cardId].second.insert(std::stoi(num));
				}
			}

			{
				int winningNumber = 0;

				for (auto &num : utils::strTok(numbers[1], ' ')) {
					num = utils::trim(num, " ");

					if (! num.empty()) {
						auto &scratchCard = scratchCards[cardId].second;

						if (scratchCard.find(std::stoi(num)) != scratchCard.end()) {
							winningNumber++;
						}
					}
				}

				if (winningNumber) {
					partA += (1 << (winningNumber - 1));

					for (int i = 0; i < winningNumber; i++) {
						auto &nextCard = scratchCards[cardId + i + 1];

						if (nextCard.first == 0) {
							nextCard.first++;
						}

						nextCard.first += (scratchCards[cardId].first);
					}
				}
			}
		}

		for (auto &c : scratchCards) {
			partB += c.second.first;
		}

		PRINTF(("PART_A: %d", partA));
		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
