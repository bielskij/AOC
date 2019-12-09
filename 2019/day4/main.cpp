#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

#include "common/debug.h"


int main(int argc, char *argv[]) {
	int start = atoi(argv[1]);
	int end   = atoi(argv[2]);

	{
		int sumNumbers = 0;

		for (int i = start; i <= end; i++) {
			char str[7];

			snprintf(str, sizeof(str), "%d", i);

			bool hasPair = false;
			int j;

			for (j = 0; j < strlen(str) - 1; j++) {
				if (str[j + 1] < str[j]) {
					break;
				}

				if (str[j] == str[j + 1]) {
					hasPair = true;
				}
			}

			if ((j == (strlen(str) - 1)) && hasPair) {
				sumNumbers++;
			}
		}

		PRINTF(("PART1: %d", sumNumbers));
	}

	{
		int sumNumbers = 0;

		for (int i = start; i <= end; i++) {
			char str[7];

			snprintf(str, sizeof(str), "%d", i);

			std::map<char, int> letters;
			int j;

			for (j = 0; j < strlen(str); j++) {
				if (j < strlen(str) - 1) {
					if (str[j + 1] < str[j]) {
						break;
					}
				}

				letters[str[j]]++;
			}

			if (j == (strlen(str))) {
				bool hasPair = false;

				for (auto it = letters.begin(); it != letters.end(); it++) {
					if (it->second == 2) {
						hasPair = true;
					}
				}

				if (hasPair) {
					sumNumbers++;
				}
			}
		}

		PRINTF(("PART 2: %d", sumNumbers));
	}
}
