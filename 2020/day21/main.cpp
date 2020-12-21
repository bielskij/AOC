#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<std::string, int>                   ingreedientsFrequency;
	std::map<std::string, std::set<std::string>> alergensMap;

	for (auto &line : lines) {
		auto ingredients = utils::strTok(utils::trim(line.substr(0, line.find('(')), " "), ' ');
		auto alergens    = utils::strTok(line.substr(line.find("(contains") + 10, line.size() - line.find("(contains") - 11), ',');

		for (auto &ing : ingredients) {
			ingreedientsFrequency[ing]++;
		}

		for (auto &alergen : alergens) {
			alergen = utils::trim(alergen, " ");

			if (alergensMap[alergen].empty()) {
				for (auto &ing : ingredients) {
					alergensMap[alergen].insert(ing);
				}

			} else {
				std::set<std::string> newIngredients;

				for (auto &ing : ingredients) {
					auto it = alergensMap[alergen].find(ing);

					if (it != alergensMap[alergen].end()) {
						newIngredients.insert(ing);
					}
				}

				alergensMap[alergen] = newIngredients;
			}
		}
	}

	// solve alergens
	{
		bool solved = false;

		while (! solved) {
			for (auto alergenIt = alergensMap.begin(); alergenIt != alergensMap.end(); alergenIt++) {
				if (alergenIt->second.size() == 1) {
					const std::string &alergen     =  alergenIt->first;
					const std::string &ingreedient = *alergenIt->second.begin();

					for (auto it = alergensMap.begin(); it != alergensMap.end(); it++) {
						if (it->first == alergen) {
							continue;
						}

						if (it->second.size() == 1) {
							continue;
						}

						auto rit = it->second.find(ingreedient);
						if (rit != it->second.end()) {
							it->second.erase(rit);
						}
					}
				}
			}

			solved = true;
			for (auto alergenIt = alergensMap.begin(); alergenIt != alergensMap.end(); alergenIt++) {
				if (alergenIt->second.size() > 1) {
					solved = false;
					break;
				}
			}
		}
	}

	for (auto &alergen : alergensMap) {
		PRINTF(("Alergen: %s", alergen.first.c_str()));
		for (auto &ingredient : alergen.second) {
			PRINTF(("  %s", ingredient.c_str()));
		}
	}
	PRINTF(("\n"));

	{
		int partA = 0;

		for (auto &ing : ingreedientsFrequency) {
			bool isAlergen = false;

			for (auto &alergen : alergensMap) {
				if (alergen.second.find(ing.first) != alergen.second.end()) {
					isAlergen = true;
					break;
				}
			}

			if (! isAlergen) {
				partA += ing.second;
			}
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		std::string partB;

		for (auto alergen : alergensMap) {
			if (! partB.empty()) {
				partB += ",";
			}

			partB += *alergen.second.begin();
		}

		PRINTF(("PART_B: %s", partB.c_str()));
	}

	return 0;
}
