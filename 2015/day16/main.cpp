#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	std::map<std::string, int> tickerTape;

	tickerTape["children"]    = 3;
	tickerTape["cats"]        = 7;
	tickerTape["samoyeds"]    = 2;
	tickerTape["pomeranians"] = 3;
	tickerTape["akitas"]      = 0;
	tickerTape["vizslas"]     = 0;
	tickerTape["goldfish"]    = 5;
	tickerTape["trees"]       = 3;
	tickerTape["cars"]        = 2;
	tickerTape["perfumes"]    = 1;

	std::vector<std::map<std::string, int>> sues;

	for (auto l : data) {
		char el1[64], el2[64], el3[64];
		int  idx, el1Val, el2Val, el3Val;

		int scanfret = sscanf(l.c_str(), "Sue %d: %[^:]: %d, %[^:]: %d, %[^:]: %d", &idx, el1, &el1Val, el2, &el2Val, el3, &el3Val);
		if (scanfret == 7) {
			std::map<std::string, int> newElement;

			newElement[el1] = el1Val;
			newElement[el2] = el2Val;
			newElement[el3] = el3Val;

			sues.push_back(newElement);

		} else {
			PRINTF(("%d", scanfret));
			abort();
		}
	}

	int idx = 0;

	int partAFit    = 0;
	int partAFitIdx = 0;
	int partBFit    = 0;
	int partBFitIdx = 0;

	for (auto &sue : sues) {
		int cA = 0;
		int cB = 0;

		idx++;

		for (auto &attr : sue) {
			if (tickerTape.find(attr.first) != tickerTape.end()) {
				if (tickerTape[attr.first] == attr.second) {
					cA++;
				}

				if (attr.first.compare("cats") == 0 || attr.first.compare("trees") == 0) {
					if (tickerTape[attr.first] < attr.second) {
						cB++;
					}

				} else if (attr.first.compare("pomeranians") == 0 || attr.first.compare("goldfish") == 0) {
					if (tickerTape[attr.first] > attr.second) {
						cB++;
					}

				} else {
					if (tickerTape[attr.first] == attr.second) {
						cB++;
					}
				}

			} else {
				abort();
			}
		}

		if (cA >= partAFit) {
			partAFit    = cA;
			partAFitIdx = idx;
		}

		if (cB >= partBFit) {
			partBFit    = cB;
			partBFitIdx = idx;
		}
	}

	PRINTF(("PART_A: %d", partAFitIdx));
	PRINTF(("PART_B: %d", partBFitIdx));
}
