#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Spell {
	int  manaCost;
	int  manaProfit;
	int  time;
	int  damage;
	int  hit;
	int  armor;

	Spell() {
		this->manaCost   = 0;
		this->manaProfit = 0;
		this->time       = 0;
		this->damage     = 0;
		this->hit        = 0;
		this->armor      = 0;
	}

	Spell(int manaCost, int manaProfit, int time, int damage, int hit, int armor) {
		this->manaCost   = manaCost;
		this->manaProfit = manaProfit;
		this->time       = time;
		this->damage     = damage;
		this->hit        = hit;
		this->armor      = armor;
	}
};


struct Player {
	int mana;
	int hit;
	int armor;
	int damage;

	std::map<std::string, int> spells;

	Player() {
		this->mana   = 0;
		this->hit    = 0;
		this->armor  = 0;
		this->damage = 0;
	}

	Player(int hit, int mana) {
		this->hit    = hit;
		this->mana   = mana;
		this->armor  = 0;
		this->damage = 0;
	}
};


static void _commitSpell(Player &user, Player &boss, const Spell &spell) {
	user.mana += spell.manaProfit;
	user.hit  += spell.hit;

	boss.hit  -= spell.damage;
}


static void _turn(const std::map<std::string, Spell> &spells, Player &user, Player &boss, int userDecrease, bool userTurn, int manaUsed, int &min) {
	bool anyOneLost = false;

	// Ignore fights overflowing current min mana
	if (manaUsed > min) {
		return;
	}

	user.hit -= userDecrease;

	// Apply effects
	if (! anyOneLost) {
		std::map<std::string, int>::iterator s = user.spells.begin();
		while (s != user.spells.end()) {
			const Spell &spell = spells.at(s->first);

			_commitSpell(user, boss, spell);

			// Timer
			s->second++;

			if (s->second == spell.time) {
				s = user.spells.erase(s);

				user.armor -= spell.armor;

			} else {
				s++;
			}
		}

		if (boss.hit <= 0 || user.hit <= 0) {
			anyOneLost = true;
		}
	}

	if (! anyOneLost) {
		if (userTurn) {
			std::vector<std::string> possibleSpells;

			// Possible spells
			for (auto &s : spells) {
				if (s.second.manaCost <= user.mana) {
					if (user.spells.find(s.first) == user.spells.end()) {
						possibleSpells.push_back(s.first);
					}
				}
			}

			if (possibleSpells.size() == 0) {
				// User has not enough mana
//				anyOneLost = true;

			} else {
				int    outMana = manaUsed;
				Player outBoss = boss;

				for (auto &spellName : possibleSpells) {
					const Spell &toCast = spells.at(spellName);

					Player nextUser = user;
					Player nextBoss = boss;
					int    nextMana = manaUsed;

					nextUser.mana -= toCast.manaCost;
					nextMana      += toCast.manaCost;

					if (toCast.time == 0) {
						_commitSpell(nextUser, nextBoss, toCast);

					} else {
						nextUser.spells[spellName] = 0;

						nextUser.armor += toCast.armor;
					}

					if (nextBoss.hit > 0) {
						_turn(spells, nextUser, nextBoss, userDecrease, false, nextMana, min);

					} else {
						outMana    = nextMana;
						outBoss    = nextBoss;
						anyOneLost = true;
					}
				}

				if (anyOneLost) {
					manaUsed = outMana;
					boss     = outBoss;
				}
			}

		} else {
			int damage = std::max(1, boss.damage - user.armor);

			user.hit -= damage;
			if (user.hit <= 0) {
				anyOneLost = true;

			} else {
				_turn(spells, user, boss, userDecrease, true, manaUsed, min);
			}
		}
	}

	if (anyOneLost) {
		if (boss.hit <= 0 && user.hit > 0) {
			if (manaUsed < min) {
				min = manaUsed;
			}
		}
	}
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	Player user(50, 500);
	Player boss; // params from input

	std::map<std::string, Spell> spells;

	for (auto l : data) {
		if (
			(sscanf(l.c_str(), "Hit Points: %d", &boss.hit) != 1) &&
			(sscanf(l.c_str(), "Damage: %d", &boss.damage)  != 1)
		) {
			abort();
		}
	}

//	Spell(int manaCost, int manaProfit, int time, int damage, int hit, int armor) {
	spells["Magic Missile"] = Spell( 53,   0, 0, 4, 0, 0);
	spells["Drain"]         = Spell( 73,   0, 0, 2, 2, 0);
	spells["Shield"]        = Spell(113,   0, 6, 0, 0, 7);
	spells["Poison"]        = Spell(173,   0, 6, 3, 0, 0);
	spells["Recharge"]      = Spell(229, 101, 5, 0, 0, 0);

	int minMana;

	minMana = std::numeric_limits<int>::max();
	_turn(spells, user, boss, 0, true, 0, minMana);
	PRINTF(("PART_A: %d", minMana));

	minMana = std::numeric_limits<int>::max();
	_turn(spells, user, boss, 1, true, 0, minMana);
	PRINTF(("PART_B: %d", minMana));
}
