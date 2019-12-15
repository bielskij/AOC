#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Recipe {
	int produced;
	std::map<std::string, int> materials;
};


static long long int getRequiredOre(std::set<std::string> &materials, std::map<std::string, Recipe> &recipes, std::map<std::string, int> &distance, long long int fuelCount) {
	std::map<std::string, long long int> needed;

	needed["FUEL"] = fuelCount;

	while (needed.size() > 1 || needed.find("ORE") == needed.end()) {
		std::string   material;
		long long int materialCount;

		// Find one with maximal distance
		{
			long long int max = INT_MIN;

			for (auto n = needed.begin(); n != needed.end(); n++) {
				if (distance[n->first] > max) {
					max = distance[n->first];
					material = n->first;
				}
			}
		}

		materialCount = needed[material];
		needed.erase(material);

		if (material.compare("ORE") == 0) {
			needed[material] = materialCount;

			continue;
		}

		for (auto i = recipes[material].materials.begin(); i != recipes[material].materials.end(); i++) {
			if (needed.find(i->first) == needed.end()) {
				needed[i->first] = 0;
			}

			needed[i->first] += ceil((double)materialCount / (double)recipes[material].produced) * (long long int) i->second;
		}
	}

	return needed["ORE"];
}


int main(int argc, char *argv[]) {
	std::set<std::string>         materials;
	std::map<std::string, Recipe> recipes;
	std::map<std::string, int>    distance;

	{
		auto data = File::readAllLines(argv[1]);

		std::set<std::string> keys;

		for (auto line = data.begin(); line != data.end(); line++) {
			auto items = utils::strTok(*line, '=');

			{
				auto outs      = utils::strTok(items[1], ' ');
				int  outFactor = utils::toInt(outs[1]);

				materials.insert(outs[2]);

				recipes[outs[2]].produced = outFactor;

				auto pairs = utils::strTok(items[0], ',');
				for (auto pair = pairs.begin(); pair != pairs.end(); pair++) {
					auto values = utils::strTok(utils::trim(*pair, " "), ' ');

					recipes[outs[2]].materials[values[1]] = utils::toInt(values[0]);
				}
			}
		}

#if 0
		for (auto r = recipes.begin(); r != recipes.end(); r++) {
			PRINTF(("%d %s", r->second.produced, r->first.c_str()));

			for (auto i = r->second.materials.begin(); i != r->second.materials.end(); i++) {
				PRINTF(("   %d %s", i->second, i->first.c_str()));
			}
		}
#endif
		materials.insert("ORE");

		distance["ORE"] = 0;

		while (distance.size() != materials.size()) {
			for (auto material = materials.begin(); material != materials.end(); material++) {
				bool ignore = false;

				if (distance.find(*material) != distance.end()) {
					ignore = true;
				}

				// Check all dependencies
				if (! ignore) {
					for (auto i = recipes[*material].materials.begin(); i != recipes[*material].materials.end(); i++) {
						if (distance.find(i->first) == distance.end()) {
							ignore = true;
							break;
						}
					}
				}

				if (! ignore) {
					int max = 0;

					for (auto i = recipes[*material].materials.begin(); i != recipes[*material].materials.end(); i++) {
						if (distance[i->first] > max) {
							max = distance[i->first];
						}
					}

					distance[*material] = max + 1;
				}
			}
		}
	}

	{
		long long int oreCount = 1000000000000LL;
		long long int orePerFuel = getRequiredOre(materials, recipes, distance, 1);

		long long int target   = oreCount / orePerFuel;
		long long int consumed = getRequiredOre(materials, recipes, distance, target);

		while (1) {
			target += ((oreCount - consumed) / orePerFuel + 1);

			consumed = getRequiredOre(materials, recipes, distance, target);

			if (consumed > oreCount) {
				break;
			}
		}

		PRINTF(("PART_A: %lld", orePerFuel));
		PRINTF(("PART_B: %lld", target - 1));
	}
}
