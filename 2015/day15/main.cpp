#include <cstring>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Recipe {
	int ingredients[5];

	Recipe() {
		memset(ingredients, 0, sizeof(ingredients));
	}
};


void _iterate(std::vector<std::pair<std::string, Recipe>> &recipes, int *val, int valSize, int rest, int depth, int &max) {
	if (depth < valSize - 1) {
		for (int i = 0; i <= rest; i++) {
			val[depth] = i;

			_iterate(recipes, val, valSize, rest - i, depth + 1, max);
		}

	} else {
		int score = 1;

		val[depth] = rest;

		for (int j = 0; j < 4; j++) {
			int sum = 0;

			for (int i = 0; i < valSize; i++) {
				sum += (val[i] * recipes[i].second.ingredients[j]);
			}

			if (sum < 0) {
				sum = 0;
			}
			score *= sum;
		}

		if (score > max) {
			max = score;
		}
	}
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	std::vector<std::pair<std::string, Recipe>> ingreedients;

	// Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories 3
	for (auto l : data) {
		char   name[64];
		Recipe recipe;

		if (sscanf(l.c_str(), "%[^:]: capacity %d, durability %d, flavor %d, texture %d, calories %d",
			name, &recipe.ingredients[0], &recipe.ingredients[1], &recipe.ingredients[2],
			&recipe.ingredients[3], &recipe.ingredients[4]
			) != 6
		) {
			abort();
		}

		ingreedients.push_back(std::pair<std::string, Recipe>(name, recipe));
	}

	{
		int maxScore = 0;

		int factors[ingreedients.size()];

		_iterate(ingreedients, factors, ingreedients.size(), 100, 0, maxScore);

		PRINTF(("PART_A: %d", maxScore));
	}
}
