#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<int, std::vector<std::map<std::string, int>>> data;

	for (const auto &line : lines) {
		auto gameDescription = utils::strTok(line, ':');

		auto &game = data[std::stoi(utils::strTok(gameDescription[0], ' ')[1])];

		auto turnes = utils::strTok(gameDescription[1], ';');

		for (auto turn : turnes) {
			auto cubeSetStr = utils::strTok(utils::trim(turn, " "), ',');

			std::map<std::string, int> cubeSet;

			for (auto &cube : cubeSetStr) {
				auto cubeAmount = utils::strTok(utils::trim(cube, " "), ' ');

				cubeSet[cubeAmount[1]] = std::stoi(cubeAmount[0]);
			}

			game.push_back(cubeSet);
		}
	}

	{
		int partA = 0;

		std::map<std::string, int> result = {
			{ "red", 12 },
			{ "green", 13 },
			{ "blue", 14 }
		};

		for (const auto &game : data) {
			bool pass = true;

			for (auto &turn : game.second) {
				for (auto &cube : turn) {
					if (result[cube.first] < cube.second) {
						pass = false;
						break;
					}
				}
			}

			if (pass) {
				partA += game.first;
			}
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		int partB = 0;

		for (const auto &game : data) {
			std::map<std::string, int> cubes;

			for (const auto &turn : game.second) {
				for (const auto &cube : turn) {
					if (cubes[cube.first] < cube.second) {
						cubes[cube.first] = cube.second;
					}
				}
			}

			{
				int power = 1;

				for (const auto &c : cubes) {
					power *= c.second;
				}

				partB += power;
			}
		}

		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
