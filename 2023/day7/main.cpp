#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static const std::string symbolsA = "AKQJT98765432";
static const std::string symbolsB = "AKQT98765432J";


static int getStrength(const std::string &symbols, char c) {
	return symbols.length()- symbols.find_first_of(c);
}


enum class Type {
	NONE,

	HIGH_CARD,
	ONE_PAIR,
	TWO_PAIR,
	THREE_OF_KIND,
	FULL_HOUSE,
	FOUR_OF_KIND,
	FIVE_OF_KIND
};


struct Hand {
	Hand(const std::string &desc, const std::string &bid, const std::string &symbolOrder, char jokerSymbol) {
		this->bid  = std::stoi(bid);
		this->type = Type::NONE;
		this->desc = desc;

		{
			std::unordered_map<char, int> symbolsMap;

			int jokers = 0;

			for (auto c : desc) {
				if (c != jokerSymbol) {
					symbolsMap[c]++;

				} else {
					jokers++;
				}
			}

			if (type == Type::NONE) {
				if (jokers == 5) {
					this->type = Type::FIVE_OF_KIND;

				} else {
					for (auto &freq : symbolsMap) {
						if (freq.second + jokers == 5) {
							this->type = Type::FIVE_OF_KIND;
							break;
						}
					}
				}
			}

			if (type == Type::NONE) {
				for (auto &freq : symbolsMap) {
					if (freq.second + jokers == 4) {
						this->type = Type::FOUR_OF_KIND;
						break;
					}
				}
			}

			if (type == Type::NONE) {
				int threeCount = 0;
				int twoCount   = 0;

				for (auto &freq : symbolsMap) {
					if (freq.second == 3) {
						threeCount++;

					} else if (freq.second == 2) {
						twoCount++;
					}
				}

				if (threeCount == 1) {
					if (twoCount == 1) {
						this->type = Type::FULL_HOUSE;
					} else {
						if (jokers) {
							this->type = Type::FULL_HOUSE;

						} else {
							this->type = Type::THREE_OF_KIND;
						}
					}

				} else {
					if (twoCount == 2) {
						if (jokers) {
							this->type = Type::FULL_HOUSE;

						} else {
							this->type = Type::TWO_PAIR;
						}

					} else if (twoCount == 1) {
						if (jokers) {
							this->type = Type::THREE_OF_KIND;

						} else {
							this->type = Type::ONE_PAIR;
						}

					} else {
						if (jokers == 2) {
							this->type = Type::THREE_OF_KIND;

						} else if (jokers == 1) {
							this->type = Type::ONE_PAIR;

						} else {
							auto index = symbolOrder.find_first_of(desc[0]);

							bool highCard = true;
							for (int i = 0; i < desc.size(); i++) {
								if (symbolOrder[index + i] != desc[i]) {
									highCard = false;
									break;
								}
							}

							if (highCard) {
								this->type == Type::HIGH_CARD;
							}
						}
					}
				}
			}
		}
	}

	Type getType() {
		return this->type;
	}

	int getBid() {
		return this->bid;
	}

	int  bid;
	Type type;
	std::string desc;
};


int solve(const std::vector<std::string> &lines, bool partA) {
	int ret = 0;

	{
		std::vector<Hand> hands;

		const std::string &symbols = partA ? symbolsA : symbolsB;

		for (const auto &l : lines) {
			auto toks = utils::strTok(l, ' ');

			hands.push_back(Hand(toks[0], toks[1], symbols, partA ? '-' : 'J'));
		}

		std::sort(hands.begin(), hands.end(), [&symbols](const Hand &a, const Hand &b) {
			if (a.type < b.type) {
				return true;

			} else if (a.type == b.type) {
				int idx = 0;

				while (a.desc[idx] == b.desc[idx]) {
					idx++;
				}

				if (getStrength(symbols, a.desc[idx]) < getStrength(symbols, b.desc[idx])) {
					return true;
				}
			}

			return false;
		});

//		for (const auto &hand : hands) {
//			PRINTF(("%s, type: %d", hand.desc.c_str(), hand.type));
//		}

		for (int i = 0; i < hands.size(); i++) {
			ret += ((i + 1) * hands[i].bid);
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, true)));
	PRINTF(("PART_B: %d", solve(lines, false)));

	return 0;
}
