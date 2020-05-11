#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Disc {
	unsigned int id;
	unsigned int positions;
	unsigned int position;

	Disc() {
		this->id        = 0;
		this->positions = 0;
		this->position  = 0;
	}

	Disc(int id, int positions, int position) {
		this->id        = id;
		this->positions = positions;
		this->position  = position;
	}

	void tick(int steps) {
		this->position = (this->position + steps) & this->positions;
	}
};


std::vector<std::pair<int64_t, int64_t>> mapDiscsToVector(const std::vector<Disc> &discs) {
	std::vector<std::pair<int64_t, int64_t>> ret;

	ret.reserve(discs.size());

	for (int idx = 0; idx < discs.size(); idx++) {
		const Disc &d = discs[idx];

		int num = d.positions;
		int rem = -(d.position + idx + 1);

		while (rem < 0) {
			rem += num;
		}

		ret.push_back(std::pair<int64_t, int64_t>(num, rem));
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<Disc> discs;

	for (auto &l : lines) {
		Disc d;

		if (sscanf(l.c_str(), "Disc #%u has %u positions; at time=0, it is at position %u.",
			&d.id, &d.positions, &d.position) != 3
		) {
			ERR(("Unable to parse input line!"));

			exit (1);
		}

		discs.push_back(d);
	}

	PRINTF(("PART_A: %" PRId64, utils::getMinXCRD(mapDiscsToVector(discs))));

	// PART B
	discs.push_back(Disc(-1, 11, 0));
	PRINTF(("PART_B: %" PRId64, utils::getMinXCRD(mapDiscsToVector(discs))));
}
