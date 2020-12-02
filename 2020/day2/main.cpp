#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int partA = 0;
	int partB = 0;

	for (auto &line : lines) {
		int  charMin;
		int  charMax;
		char character;
		char password[64];

		if (sscanf(line.c_str(), "%d-%d %c: %64s", &charMin, &charMax, &character, password) != 4) {
			ERR(("scanf failed!"));
			return 1;
		}

		size_t cn = std::count(password, password + strlen(password), character);

		if (cn >= charMin && cn <= charMax) {
			partA++;
		}

		charMin--;
		charMax--;

		if (
			(password[charMin] == character && password[charMax] != character) ||
			(password[charMin] != character && password[charMax] == character)
		) {
			partB++;
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB)); // 274, 147
}
