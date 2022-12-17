#include <cassert>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Shape {
	std::vector<uint8_t> pattern;
	int maxShiftLeft;
	int maxShiftRight;
};

static const std::vector<Shape> shapes = {
	{
		{
			0b00011110
		}, 2, -1
	},
	{
		{
			0b00001000,
			0b00011100,
			0b00001000
		}, 2, -2
	},
	{
		{
			0b00000100,
			0b00000100,
			0b00011100
		}, 2, -2
	},
	{
		{
			0b00010000,
			0b00010000,
			0b00010000,
			0b00010000
		}, 2, -4
	},
	{
		{
			0b00011000,
			0b00011000
		}, 2, -3
	}
};


static uint8_t getShifted(uint8_t val, int shift) {
	if (shift == 0) {
		return val;
	}

	if (shift > 0) {
		return val << shift;
	}

	return val >> -shift;
}


static bool checkFit(const std::vector<uint8_t> &chamber, int topToCheck, const Shape &shape, int shift) {
	if (chamber.size() > 0) {
		for (int idx = 0; idx < shape.pattern.size(); idx++) {
			auto chamberIdx = topToCheck + idx;

			if (
				(chamberIdx < chamber.size()) &&
				((chamber[chamberIdx] & getShifted(shape.pattern[shape.pattern.size() - idx - 1], shift)) != 0)
			) {
				return false;
			}
		}
	}

	return true;
}


static int64_t solve(const std::vector<std::string> &lines, bool partA) {
	{
		std::vector<uint8_t> chamber;

		auto const &wind = lines[0];

		int shapeIdx     = 0;
		int windIdx      = 0;

		chamber.push_back(0xff);

		std::vector<int> chamberHeights;
		std::map<std::pair<int, int>, std::vector<int>> drops;
		std::map<int, int> dropLoop;


		while (true) {
			int  newChamberSize = chamber.size() + 3;
			int  shapeShift     = 0;
			bool shapeCanDrop   = true;

			const Shape &shape = shapes[shapeIdx % shapes.size()];

			do {
				switch (wind[windIdx++]) {
					case '<':
						if (shapeShift != shape.maxShiftLeft && checkFit(chamber, newChamberSize, shape, shapeShift + 1)) {
							shapeShift++;
						}
						break;

					case '>':
						if (shapeShift != shape.maxShiftRight && checkFit(chamber, newChamberSize, shape, shapeShift - 1)) {
							shapeShift--;
						}
						break;

					default:
						assert(false);
				}

				windIdx %= wind.size();

				shapeCanDrop = checkFit(chamber, newChamberSize - 1, shape, shapeShift);
				if (shapeCanDrop) {
					newChamberSize--;
				}
			} while (shapeCanDrop);

			{
				int desiredChamberSize = newChamberSize + shape.pattern.size();

				if (chamber.size() < desiredChamberSize) {
					chamber.resize(desiredChamberSize);
				}

				for (int idx = 0; idx < shape.pattern.size(); idx++) {
					chamber[newChamberSize + idx] |= getShifted(shape.pattern[shape.pattern.size() - idx - 1], shapeShift);
				}

				shapeIdx++;
			}

			if (partA) {
				if (shapeIdx == 2022) {
					return chamber.size() - 1;
				}

			} else {
				chamberHeights.push_back(chamber.size());

				auto checkPair = std::make_pair((shapeIdx - 1)  % shapes.size(), (windIdx + wind.size() - 1) % wind.size());

				if (drops[checkPair].size() != 0) {
					for (int x : drops[checkPair]) {
						int d = shapeIdx - 1 - x;

						dropLoop[d]++;

						if (dropLoop[d] >= 2) {
							int hd = chamberHeights[shapeIdx - 1] - chamberHeights[x];

							int64_t loops  = (1000000000000LL - x) / d;
							int64_t inLoop = 1000000000000LL - loops * d;

							return chamberHeights[inLoop] + hd * loops - 2;
						}
					}
				}
				drops[checkPair].push_back(shapeIdx - 1);
			}
		}
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %lld", solve(lines, true)));
	PRINTF(("PART_B: %lld", solve(lines, false)));

	return 0;
}
