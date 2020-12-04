#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Claim {
	int id, x, y, w, h;
};


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	std::map<int, int> area;
	std::vector<Claim> claims;

	for (auto line : data) {
		Claim claim;

		if (sscanf(line.c_str(), "#%d @ %d,%d: %dx%d", &claim.id, &claim.x, &claim.y, &claim.w, &claim.h) != 5) {
			ERR(("Parse failed!"));

			return 1;
		}

		claims.push_back(claim);

		for (int xi = claim.x; xi < claim.x + claim.w; xi++) {
			for (int yi = claim.y; yi < claim.y + claim.h; yi++) {
				int key = (yi << 15) | xi;

				area[key]++;
			}
		}
	}

	{
		int partA = 0;

		for (auto p : area) {
			if (p.second >= 2) {
				partA++;
			}
		}

		PRINTF(("PART_A: %d", partA));
	}

	for (auto &c : claims) {
		bool claimIntact = true;

		for (int xi = c.x; xi < c.x + c.w; xi++) {
			for (int yi = c.y; yi < c.y + c.h; yi++) {
				int key = (yi << 15) | xi;

				if (area[key] != 1) {
					claimIntact = false;
					break;
				}
			}
		}

		if (claimIntact) {
			PRINTF(("PART_B: %d", c.id));
			break;
		}
	}

	return 0;
}
