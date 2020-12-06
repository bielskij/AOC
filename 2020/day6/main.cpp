#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Group {
	std::map<char, int> questions;
	int people;

	Group() {
		this->people = 0;
	}
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<Group *> groups;

	Group *currentGroup = nullptr;

	for (auto &l : lines) {
		if (l.empty()) {
			if (currentGroup) {
				groups.push_back(currentGroup);

				currentGroup = nullptr;
			}

			continue;
		}

		if (currentGroup == nullptr) {
			currentGroup = new Group();
		}

		for (auto c : l) {
			currentGroup->questions[c]++;
		}

		currentGroup->people++;
	}

	groups.push_back(currentGroup);

	{
		int partA = 0;

		for (auto g : groups) {
			partA += g->questions.size();
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		int partB = 0;

		for (auto g : groups) {
			for (auto c : g->questions) {
				if (c.second == g->people) {
					partB++;
				}
			}
		}

		PRINTF(("PART_B: %d", partB));
	}
}
