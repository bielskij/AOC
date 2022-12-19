#include <cassert>
#include <cstring>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


enum ResourceType {
	FIRST = 0,

	ORE   = FIRST,
	CLAY,
	OBSIDIAN,
	GEODE,

	LAST
};


struct Blueprint {
	std::array<std::array<int, 4>, 4> costs;

	std::array<int, 4> maxUsedLut;

	int getMaxUsed(ResourceType type) {
		if (maxUsedLut[0] == 0) {
			for (int i = 0; i < ResourceType::LAST; i++) {
				int max = 0;

				for (auto c : costs) {
					if (c[i] > max) {
						max = c[i];
					}
				}

				maxUsedLut[i] = max;
			}
		}

		return maxUsedLut[type];
	}
};


static ResourceType strToResource(const std::string &str) {
	if (str == "ore") {
		return ResourceType::ORE;

	} else if (str == "clay") {
		return ResourceType::CLAY;

	} else if (str == "obsidian") {
		return ResourceType::OBSIDIAN;

	} else if (str == "geode") {
		return ResourceType::GEODE;
	}

	assert(false);

	return ResourceType::LAST;
}


static int estimatedTime(int needed, int having) {
	return (needed + having - 1) / having;
}


int solve(Blueprint &bp, const std::array<int, ResourceType::LAST> &robots, int time, const std::array<int, ResourceType::LAST> &cargo) {
	int ret = cargo[ResourceType::GEODE] + robots[ResourceType::GEODE] * time;

	// Ore robot
	if (robots[ResourceType::ORE] < bp.getMaxUsed(ResourceType::ORE)) {
		int timeToProduce = 1;

		if (cargo[ResourceType::ORE] < bp.costs[ResourceType::ORE][ResourceType::ORE]) {
			timeToProduce += estimatedTime(
				bp.costs[ResourceType::ORE][ResourceType::ORE] - cargo[ResourceType::ORE],
				robots[ResourceType::ORE]
			);
		}

		if (time > timeToProduce) {
			auto nextTurnRobots = robots;
			auto nextTurnCargo  = cargo;

			for (int i = ResourceType::FIRST; i < ResourceType::LAST; i++) {
				nextTurnCargo[i] += (nextTurnRobots[i] * timeToProduce);
			}

			nextTurnCargo[ResourceType::ORE]  -= bp.costs[ResourceType::ORE][ResourceType::ORE];
			nextTurnRobots[ResourceType::ORE] += 1;

			ret = std::max(ret, solve(bp, nextTurnRobots, time - timeToProduce, nextTurnCargo));
		}
	}

	// Clay robot
	if (robots[ResourceType::CLAY] < bp.getMaxUsed(ResourceType::CLAY)) {
		int timeToProduce = 1;

		if (cargo[ResourceType::ORE] < bp.costs[ResourceType::CLAY][ResourceType::ORE]) {
			timeToProduce += estimatedTime(
				bp.costs[ResourceType::CLAY][ResourceType::ORE] - cargo[ResourceType::ORE],
				robots[ResourceType::ORE]
			);
		}

		if (time > timeToProduce) {
			auto nextTurnRobots = robots;
			auto nextTurnCargo  = cargo;

			for (int i = ResourceType::FIRST; i < ResourceType::LAST; i++) {
				nextTurnCargo[i] += (nextTurnRobots[i] * timeToProduce);
			}

			nextTurnCargo[ResourceType::ORE]   -= bp.costs[ResourceType::CLAY][ResourceType::ORE];
			nextTurnRobots[ResourceType::CLAY] += 1;

			ret = std::max(ret, solve(bp, nextTurnRobots, time - timeToProduce, nextTurnCargo));
		}
	}

	// Obsidian robot
	if (robots[ResourceType::CLAY] && robots[ResourceType::OBSIDIAN] < bp.getMaxUsed(ResourceType::OBSIDIAN)) {
		int timeToProduce = 1 + std::max(
			cargo[ResourceType::ORE] > bp.costs[ResourceType::OBSIDIAN][ResourceType::ORE] ?
				0 :
				estimatedTime(
					bp.costs[ResourceType::OBSIDIAN][ResourceType::ORE] - cargo[ResourceType::ORE],
					robots[ResourceType::ORE]
				),
			cargo[ResourceType::CLAY] > bp.costs[ResourceType::OBSIDIAN][ResourceType::CLAY] ?
				0 :
				estimatedTime(
					bp.costs[ResourceType::OBSIDIAN][ResourceType::CLAY] - cargo[ResourceType::CLAY],
					robots[ResourceType::CLAY]
				)
		);

		if (time > timeToProduce) {
			auto nextTurnRobots = robots;
			auto nextTurnCargo  = cargo;

			for (int i = ResourceType::FIRST; i < ResourceType::LAST; i++) {
				nextTurnCargo[i] += (nextTurnRobots[i] * timeToProduce);
			}

			nextTurnCargo[ResourceType::ORE]  -= bp.costs[ResourceType::OBSIDIAN][ResourceType::ORE];
			nextTurnCargo[ResourceType::CLAY] -= bp.costs[ResourceType::OBSIDIAN][ResourceType::CLAY];

			nextTurnRobots[ResourceType::OBSIDIAN] += 1;

			ret = std::max(ret, solve(bp, nextTurnRobots, time - timeToProduce, nextTurnCargo));
		}
	}

	// Geode robot
	if (robots[ResourceType::OBSIDIAN]) {
		int timeToProduce = 1 + std::max(
			cargo[ResourceType::ORE] > bp.costs[ResourceType::GEODE][ResourceType::ORE] ?
				0 :
				estimatedTime(
					bp.costs[ResourceType::GEODE][ResourceType::ORE] - cargo[ResourceType::ORE],
					robots[ResourceType::ORE]
				),
			cargo[ResourceType::OBSIDIAN] > bp.costs[ResourceType::GEODE][ResourceType::OBSIDIAN] ?
				0 :
				estimatedTime(
					bp.costs[ResourceType::GEODE][ResourceType::OBSIDIAN] - cargo[ResourceType::OBSIDIAN],
					robots[ResourceType::OBSIDIAN]
				)
		);

		if (time > timeToProduce) {
			auto nextTurnRobots = robots;
			auto nextTurnCargo  = cargo;

			for (int i = ResourceType::FIRST; i < ResourceType::LAST; i++) {
				nextTurnCargo[i] += (nextTurnRobots[i] * timeToProduce);
			}

			nextTurnCargo[ResourceType::ORE]      -= bp.costs[ResourceType::GEODE][ResourceType::ORE];
			nextTurnCargo[ResourceType::OBSIDIAN] -= bp.costs[ResourceType::GEODE][ResourceType::OBSIDIAN];

			nextTurnRobots[ResourceType::GEODE] += 1;

			ret = std::max(ret, solve(bp, nextTurnRobots, time - timeToProduce, nextTurnCargo));
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Blueprint> blueprints;

		for (auto &l : lines) {
			auto costString = utils::strTok(l.substr(l.find(':') + 2), '.');

			Blueprint p;

			for (auto &c : costString) {
				c = utils::trim(c, " ");

				if (c.empty()) {
					continue;
				}

				{
					char resType1Str[16];
					int  resType1Cnt;
					char resType2Str[16];
					int  resType2Cnt;

					assert(sscanf(c.c_str(), "Each %s robot costs", resType1Str) == 1);

					ResourceType robotType = strToResource(resType1Str);

					{
						auto s = c.substr(18 + strlen(resType1Str));

						if (
							sscanf(s.c_str(), "%d %s and %d %s", &resType1Cnt, resType1Str, &resType2Cnt, resType2Str) == 4
						) {
							p.costs[robotType][strToResource(resType1Str)] = resType1Cnt;
							p.costs[robotType][strToResource(resType2Str)] = resType2Cnt;

						} else if (
							sscanf(s.c_str(), "%d %s", &resType1Cnt, resType1Str) == 2
						) {
							p.costs[robotType][strToResource(resType1Str)] = resType1Cnt;

						} else {
							assert(false);
						}
					}
				}
			}

			blueprints.push_back(p);
		}

		{
			int partA = 0;

			for (int i = 0; i < blueprints.size(); i++) {
				partA += ((i + 1) * solve(blueprints[i], { 1, 0, 0, 0 }, 24, { 0, 0, 0, 0 }));
			}

			PRINTF(("PART_A: %d", partA));
		}

		{
			int partB = 1;

			for (int i = 0; i < 3; i++) {
				partB *= solve(blueprints[i], { 1, 0, 0, 0 }, 32, { 0, 0, 0, 0 });
			}

			PRINTF(("PART_B: %d", partB));
		}
	}

	return 0;
}
