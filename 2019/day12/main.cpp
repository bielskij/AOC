#include <cmath>
#include <climits>

#include <string>
#include <vector>
#include <algorithm>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Moon {
	Point3d<int> position;
	Point3d<int> velocity;

	Moon(const Point3d<int> &position) : position(position) {
	}

	int getEnergy() {
		int pot = 0;
		int kin = 0;

		for (int i = 0; i < 3; i++) {
			pot += std::abs(this->position.get(i));
			kin += std::abs(this->velocity.get(i));
		}

		return pot * kin;
	}
};


static int pairs[6][2] = {
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 3 },
	{ 1, 2 },
	{ 1, 3 },
	{ 2, 3 }
};


static void _step(std::vector<Moon> &moons) {
	// gravity
	for (int pair = 0; pair < sizeof(pairs) / sizeof(pairs[0]); pair++) {
		Moon &a = moons[pairs[pair][0]];
		Moon &b = moons[pairs[pair][1]];

		for (int i = 0; i < 3; i++) {
			if (a.position.get(i) < b.position.get(i)) {
				a.velocity.set(i, a.velocity.get(i) + 1);
				b.velocity.set(i, b.velocity.get(i) - 1);

			} else if (a.position.get(i) > b.position.get(i)) {
				a.velocity.set(i, a.velocity.get(i) - 1);
				b.velocity.set(i, b.velocity.get(i) + 1);
			}
		}
	}

	// velocity
	for (auto moon = moons.begin(); moon != moons.end(); moon++) {
		for (int i = 0; i < 3; i++) {
			moon->position.set(i, moon->position.get(i) + moon->velocity.get(i));
		}
	}
}


int main(int argc, char *argv[]) {
	std::vector<Moon> moons;

	int steps = utils::toInt(argv[2]);

	{
		auto lines = File::readAllLines(argv[1]);

		for (auto line = lines.begin(); line != lines.end(); line++) {
			int x, y, z;

			sscanf(line->c_str(), "<x=%d, y=%d, z=%d>", &x, &y, &z);

			moons.push_back(Moon(Point3d<int>(x, y, z)));
		}
	}

	{
		std::vector<Moon> m = moons;

		for (int step = 0; step < steps; step++) {
			_step(m);
		}

		int energy = 0;
		for (auto moon = m.begin(); moon != m.end(); moon++) {
			energy += moon->getEnergy();
		}

		PRINTF(("PART_A: %d", energy));
	}

	{
		std::vector<Moon> m = moons;

		int turns[3] = { -1, -1, -1 };

		long int step = 0;
		while (turns[0] == -1 || turns[1] == -1 || turns[2] == -1) {
			_step(m);
			step++;

			for (int i = 0; i < sizeof(turns) / sizeof(turns[0]); i++) {
				bool allTheSame = true;

				for (int moon = 0; moon < moons.size(); moon++) {
					if (
						(moons[moon].position.get(i) != m[moon].position.get(i)) ||
						(moons[moon].velocity.get(i) != m[moon].velocity.get(i))
					) {
						allTheSame = false;
					}
				}

				if (allTheSame) {
					if (turns[i] == -1) {
						turns[i] = step;
					}
				}
			}
		}

		PRINTF(("PART_B: %" PRId64, utils::nww<int64_t>(turns[0], utils::nww<int64_t>(turns[1], turns[2]))));
	}
}
