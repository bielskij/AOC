#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static const std::array<Point<int>, 8>adjacentMod = {
	Point<int>(-1,  1), Point<int>(0,  1), Point<int>(1,  1),
	Point<int>(-1,  0),                    Point<int>(1,  0),
	Point<int>(-1, -1), Point<int>(0, -1), Point<int>(1, -1)
};


static const std::array<std::array<Point<int>, 3>, 4> movingMod = {{
	{ Point<int>(-1, -1), Point<int>( 0, -1), Point<int>( 1, -1) }, // N
	{ Point<int>(-1,  1), Point<int>( 0,  1), Point<int>( 1,  1) }, // S
	{ Point<int>(-1,  1), Point<int>(-1,  0), Point<int>(-1, -1) }, // W
	{ Point<int>( 1,  1), Point<int>( 1,  0), Point<int>( 1, -1) }  // E
}};


struct Elf {
	Point<int> position;

	Elf(const Point<int> &p) : position(p) {
	}
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Elf> elfs;

		for (auto y = 0; y < lines.size(); y++) {
			auto &line = lines[y];

			for (auto x = 0; x < line.size(); x++) {
				if (line[x] == '#') {
					elfs.push_back(Elf(Point<int>(x, y)));
				}
			}
		}

		std::map<Point<int>, Elf *> elfMap;

		for (auto &elf : elfs) {
			elfMap[elf.position] = &elf;
		}

		for (int round = 1; round < std::numeric_limits<int>::max(); round++) {
			std::map<Point<int>, std::vector<Elf *>> moveCandidates;

			const std::array<Point<int>, 3> *firstConsideredMoveMod = nullptr;

			// first half
			for (auto &elf : elfMap) {
				bool allAdjacentsEmpty = true;

				for (const auto &adj : adjacentMod) {
					Point<int> p(elf.first.x() + adj.x(), elf.first.y() + adj.y());

					if (elfMap.find(p) != elfMap.end()) {
						allAdjacentsEmpty = false;
						break;
					}
				}

				if (allAdjacentsEmpty) {
					continue;
				}

				for (int i = 0; i < movingMod.size(); i++) {
					auto &moveVariant = movingMod.at((i + round - 1) % movingMod.size());
					bool  moveAllowed = true;

					for (auto &f : moveVariant) {
						Point<int> p(elf.second->position.x() + f.x(), elf.second->position.y() + f.y());

						if (elfMap.find(p) != elfMap.end()) {
							moveAllowed = false;
							break;
						}
					}

					if (moveAllowed) {
						moveCandidates[Point<int>(elf.first.x() + moveVariant.at(1).x(), elf.first.y() + moveVariant.at(1).y())].push_back(elf.second);

						break;
					}
				}
			}

			// second half
			{
				int movedNum = 0;

				for (auto &candidate : moveCandidates) {
					if (candidate.second.size() > 1) {
						continue;
					}

					Elf  *elfToMove = elfMap[candidate.second.at(0)->position];

					// remove
					elfMap.erase(elfToMove->position);

					// update
					elfToMove->position = candidate.first;

					// insert
					elfMap[elfToMove->position] = elfToMove;

					movedNum++;
				};

				if (movedNum == 0) {
					PRINTF(("PART_B: %d", round));
					break;
				}
			}

			// PART_A
			if (round == 10) {
				int minX = std::numeric_limits<int>::max();
				int maxX = std::numeric_limits<int>::min();
				int minY = std::numeric_limits<int>::max();
				int maxY = std::numeric_limits<int>::min();

				for (auto &elf : elfMap) {
					auto &p = elf.first;

					if (p.x() > maxX) {
						maxX = p.x();
					}

					if (p.x() < minX) {
						minX = p.x();
					}

					if (p.y() > maxY) {
						maxY = p.y();
					}

					if (p.y() < minY) {
						minY = p.y();
					}
				}

				PRINTF(("PART_A: %d", (maxX - minX + 1) * (maxY - minY + 1) - elfMap.size()));
			}
		}
	}

	return 0;
}
