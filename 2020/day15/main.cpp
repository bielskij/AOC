#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Sample {
	int history[2];
	int size;

	Sample() {
		size = 0;
	}

	Sample(int idx) {
		this->size = 0;

		this->add(idx);
	}

	void add(int idx) {
		this->history[0] = this->history[1];
		this->history[1] = idx;

		if (this->size < 2) {
			this->size++;
		}
	}
};


static int solve(std::vector<int> &samples, int turn) {
	int ret = 0;

	{
		std::map<int, Sample> history;

		int idx = 0;
		int lastSample = *samples.rbegin();

		for (auto sample : samples) {
			history[sample] = Sample(idx);

			idx++;
		}

		int val = 0;

		while (idx < turn) {
			Sample &lastTurns = history[lastSample];

			switch (lastTurns.size) {
				case 0:
					val = lastSample;
					break;

				case 1:
					val = idx - 1 - lastTurns.history[1];
					break;

				default:
					val = lastTurns.history[1] - lastTurns.history[0];
			}

			lastSample = val;

			history[val].add(idx++);
		}

		ret = lastSample;
	}

	return ret;
}


int main(int argc, char *argv[]) {
	std::vector<int> samples;

	{
		auto samplesString = utils::strTok(argv[1], ',');

		for (auto sampleString : samplesString) {
			samples.push_back(utils::toInt(sampleString));
		}
	}

	PRINTF(("PART_A: %d", solve(samples, 2020)));
	PRINTF(("PART_B: %d", solve(samples, 30000000)));

	return 0;
}
