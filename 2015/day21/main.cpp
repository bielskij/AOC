#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Item {
	std::string name;

	int cost;
	int damage;
	int armor;

	Item(const std::string &name, int cost, int damage, int armor) : name(name) {
		this->cost   = cost;
		this->damage = damage;
		this->armor  = armor;
	}

	Item() {
		this->cost   = 0;
		this->damage = 0;
		this->armor  = 0;
	}
};


struct Player {
	int hit;
	int damage;
	int armor;

	int turnHit;
	int turnDamage;
	int turnArmor;

	Item itemWeapon;
	Item itemArmor;
	Item itemsRing[2];

	Player(int hit, int damage, int armor) {
		this->hit    = hit;
		this->damage = damage;
		this->armor  = armor;

		this->reset();
	}

	Player() {
		this->hit    = 0;
		this->damage = 0;
		this->armor  = 0;

		this->reset();
	}

	void reset() {
		this->turnHit    = this->hit;
		this->turnDamage = this->damage;
		this->turnArmor  = this->armor;

		this->itemWeapon   = Item();
		this->itemArmor    = Item();
		this->itemsRing[0] = Item();
		this->itemsRing[1] = Item();
	}

	int getArmor() {
		int ret = 0;

		ret += this->turnArmor;
		ret += this->itemArmor.armor;
		ret += this->itemsRing[0].armor;
		ret += this->itemsRing[1].armor;

		return ret;
	}

	int getDamage() {
		int ret = 0;

		ret += this->turnDamage;
		ret += this->itemWeapon.damage;
		ret += this->itemsRing[0].damage;
		ret += this->itemsRing[1].damage;

		return ret;
	}

	int getCost() {
		int ret = 0;

		ret += this->itemWeapon.cost;
		ret += this->itemArmor.cost;
		ret += this->itemsRing[0].cost;
		ret += this->itemsRing[1].cost;

		return ret;
	}

	int getHit() {
		return this->turnHit;
	}

	void decreaseHit(int points) {
		this->turnHit -= points;

		if (this->turnHit < 0) {
			this->turnHit = 0;
		}
	}

	bool isLive() {
		return this->turnHit > 0;
	}
};


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	Player user(100, 0, 0);
	Player boss; // params from input

	std::vector<Item> weapons;
	std::vector<Item> armor;
	std::vector<Item> rings;

	for (auto &l : data) {
		int val;

		if (sscanf(l.c_str(), "Hit Points: %d", &val) == 1) {
			boss.hit = val;

		} else if (sscanf(l.c_str(), "Damage: %d", &val) == 1) {
			boss.damage = val;

		} else if (sscanf(l.c_str(), "Armor: %d", &val) == 1) {
			boss.armor = val;

		} else {
			abort();
		}
	}

	weapons.push_back(Item("Dagger",      8, 4, 0));
	weapons.push_back(Item("Shortsword", 10, 5, 0));
	weapons.push_back(Item("Warhammer",  25, 6, 0));
	weapons.push_back(Item("Longsword",  40, 7, 0));
	weapons.push_back(Item("Greataxe",   74, 8, 0));

	// Armor is optional
	armor.push_back(Item("None",        0, 0, 0));
	armor.push_back(Item("Leather",    13, 0, 1));
	armor.push_back(Item("Chainmail",  31, 0, 2));
	armor.push_back(Item("Splintmail", 53, 0, 3));
	armor.push_back(Item("Bandedmail", 75, 0, 4));
	armor.push_back(Item("Platemail", 102, 0, 5));

	rings.push_back(Item("Damage + 0",   0, 0, 0));
	rings.push_back(Item("Damage + 1",  25, 1, 0));
	rings.push_back(Item("Damage + 2",  50, 2, 0));
	rings.push_back(Item("Damage + 3", 100, 3, 0));
	rings.push_back(Item("Defense + 0",  0, 0, 0));
	rings.push_back(Item("Defense + 1", 20, 0, 1));
	rings.push_back(Item("Defense + 2", 40, 0, 2));
	rings.push_back(Item("Defense + 3", 80, 0, 3));

	std::vector<std::vector<Item>> ringsCombinations;

	utils::genKCombination<Item>(rings, 2, ringsCombinations);

	int minCost = std::numeric_limits<int>::max();
	int maxCost = std::numeric_limits<int>::min();
	for (int i = 0; i < weapons.size(); i++) {
		for (int j = -1; j < (int) armor.size(); j++) {
			for (int k = 0; k < ringsCombinations.size(); k++) {
				user.reset();
				boss.reset();

				user.itemWeapon = weapons[i];
				if (j >= 0) {
					user.itemArmor = armor[j];
				}

				user.itemsRing[0] = ringsCombinations[k][0];
				if (ringsCombinations[k].size() > 1) {
					user.itemsRing[1] = ringsCombinations[k][1];
				}

				bool userTurn = true;
				while (user.isLive() && boss.isLive()) {
					if (userTurn) {
						boss.decreaseHit(std::max(1, user.getDamage() - boss.getArmor()));

					} else {
						user.decreaseHit(std::max(1, boss.getDamage() - user.getArmor()));
					}

					userTurn = !userTurn;
				}

				if (! userTurn) {
					if (user.getCost() < minCost) {
						minCost = user.getCost();
					}

				} else {
					if (user.getCost() > maxCost) {
						maxCost = user.getCost();
					}
				}
			}
		}
	}

	PRINTF(("PART_A: %d", minCost));
	PRINTF(("PART_B: %d", maxCost));
}
