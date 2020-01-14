#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


void _fabricate(const std::vector<std::pair<std::string, std::string>> &rep, std::string currentMedicine, int &steps, int depth) {
	if (depth == 0) {
		steps = std::numeric_limits<int>::max();
	}

	if (currentMedicine.length() == 1) {
		if (currentMedicine[0] == 'e') {
			if (depth < steps) {
				steps = depth;
			}
		}
	}

	if (steps != std::numeric_limits<int>::max()) {
		return;
	}

	for (auto &r : rep) {
		for (size_t pos = 0; ; pos += r.second.length()) {
			std::string molecule = currentMedicine;

			pos = molecule.find(r.second, pos);
			if (pos == std::string::npos) break;

			molecule.erase(pos, r.second.length());
			molecule.insert(pos, r.first);

			_fabricate(rep, molecule, steps, depth + 1);
		}
	}
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	std::vector<std::pair<std::string, std::string>> replacements;
	std::string                                      startingString;

	for (auto l : data) {
		char src[32];
		char dst[32];

		if (l.empty()) {
			continue;
		}

		if (sscanf(l.c_str(), "%32s => %32s", src, dst) == 2) {
			replacements.push_back(std::pair<std::string, std::string>(src, dst));

		} else {
			startingString = l;
		}
	}

	{
		std::map<std::string, int> molecules;

		for (auto &r : replacements) {
			for (size_t pos = 0; ; pos += r.first.length()) {
				std::string molecule = startingString;

				pos = molecule.find(r.first, pos);
				if (pos == std::string::npos) break;

				molecule.erase(pos, r.first.length());
				molecule.insert(pos, r.second);

				molecules[molecule]++;
			}
		}

		PRINTF(("PART_A: %zd", molecules.size()));
	}

	{
		int steps = std::numeric_limits<int>::max();

		_fabricate(replacements, startingString, steps, 0);

		PRINTF(("PART_B: %d", steps));
	}
}
