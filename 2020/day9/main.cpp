#include <stdio.h>
#include <stdlib.h>

#include <numeric>
#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto samples = utils::toInt64tV(File::readAllLines(argv[1]));

	{
		std::vector<std::vector<int>> pairIdx;

		int preambleLen = 25;
		int partA;
		int partAIdx;

		std::set<int> weak;

		utils::genCombinations(pairIdx, preambleLen, 2);

		for (int i = preambleLen; i < samples.size(); i++) {
			int sample = samples[i];

			bool found = false;

			for (auto pair : pairIdx) {
				if ((samples[pair[0] - 1 + i - preambleLen] + samples[pair[1] - 1 + i - preambleLen]) == sample) {
					found = true;
					break;
				}
			}

			if (! found) {
				partA    = sample;
				partAIdx = i;
				break;
			}

			weak.insert(sample);
		}

		PRINTF(("PART_A: %d", partA));

		{
			std::vector<int> candidates;

			for (auto sample : samples) {
				if (sample < partA) {
					candidates.push_back(sample);
				}
			}

			for (int samplesCount = 2; samplesCount < candidates.size(); samplesCount++) {
				for (int sampleIdx = 0; sampleIdx < candidates.size() - samplesCount; sampleIdx++) {
					std::set<int> sumSamples(candidates.begin() + sampleIdx, candidates.begin() + sampleIdx +samplesCount);

					if (std::accumulate(sumSamples.begin(), sumSamples.end(), 0) == partA) {
						PRINTF(("PART_B: %d", *sumSamples.begin() + *sumSamples.rbegin()));
						return 1;
					}
				}
			}
		}
	}
}
