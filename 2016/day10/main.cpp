#include <stdexcept>
#include <string.h>
#include <cassert>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


struct Bot {
	std::vector<int> values;

	int  lowId;
	bool lowOut;
	int  highId;
	bool highOut;
	int  id;

	Bot() {
		this->id      = -1;
		this->lowId   = -1;
		this->highId  = -1;
		this->lowOut  = false;
		this->highOut = false;
	}

	bool compareGive(std::map<int, Bot> &bots, std::map<int, std::vector<int>> &outputs) {
		if (this->values.size() == 2) {
			int lo = this->values[0] > this->values[1] ? this->values[1] : this->values[0];
			int hi = lo == this->values[1] ? this->values[0] : this->values[1];

			this->values.clear();

			if (hi == 61 && lo == 17) {
				PRINTF(("PART_A: %d", this->id));
			}

			if (this->highOut) {
				outputs[this->highId].push_back(hi);

			} else {
				bots[this->highId].values.push_back(hi);
			}

			if (this->lowOut) {
				outputs[this->lowId].push_back(lo);

			} else {
				bots[this->lowId].values.push_back(lo);
			}

			return true;
		}

		return false;
	}
};


int main(int argc, char *argv[]) {
	auto instructions = File::readAllLines(argv[1]);

	std::map<int, Bot> bots;
	std::map<int, std::vector<int>> outputs;

	for (auto &instruction : instructions) {
		int a, b, c;

		if (sscanf(instruction.c_str(), "bot %d gives low to bot %d and high to bot %d", &a, &b, &c) == 3) {
			bots[a].id     = a;
			bots[a].lowId  = b;
			bots[a].highId = c;

			assert(! bots[a].lowOut);
			assert(! bots[a].highOut);

		} else if (sscanf(instruction.c_str(), "value %d goes to bot %d", &a, &b) == 2) {
			bots[a].id     = a;
			bots[b].values.push_back(a);

		} else if (sscanf(instruction.c_str(), "bot %d gives low to output %d and high to bot %d", &a, &b, &c) == 3) {
			bots[a].id     = a;
			bots[a].lowId  = b;
			bots[a].lowOut = true;
			bots[a].highId = c;

			assert(! bots[a].highOut);

		} else if (sscanf(instruction.c_str(), "bot %d gives low to output %d and high to output %d", &a, &b, &c) == 3) {
			bots[a].id      = a;
			bots[a].lowId   = b;
			bots[a].lowOut  = true;
			bots[a].highId  = c;
			bots[a].highOut = true;

		} else {
			abort();
		}
	}

	bool anyChanged;
	do {
		anyChanged = false;

		for (auto &bot : bots) {
			anyChanged |= bot.second.compareGive(bots, outputs);
		}
	} while (anyChanged);

	PRINTF(("PART_B: %d", outputs[0][0] * outputs[1][0] * outputs[2][0]));

}
