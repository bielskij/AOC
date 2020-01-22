#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


bool isAbba(const std::string &str) {
	if (str.size() >= 4) {
		for (int i = 0; i < str.size() - 3; i++) {
			if (
				(str[i] == str[i + 3]) && (str[i + 1] == str[i + 2]) && (str[i] != str[i + 1])
			) {
				return true;
			}
		}
	}

	return false;
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	int partA = 0;

	for (auto &l : data) {
		std::string word;
		bool ishypernet = false;

		bool noAbbaBeforeSquare = true;
		bool abbaInHypernet     = false;

		for (int i = 0; i < l.length(); i++) {
			char c = l[i];

			if (c == '[') {
				PRINTF(("%s - %d", word.c_str(), isAbba(word)));
				if (! word.empty() && isAbba(word)) {
					noAbbaBeforeSquare = false;
				}

				ishypernet = true;
				word.clear();

			} else if (c == ']') {
				PRINTF(("%s - %d", word.c_str(), isAbba(word)));
				if (! word.empty() && isAbba(word)) {
					abbaInHypernet = true;
				}

				ishypernet = false;
				word.clear();

			} else {
				word += c;
			}

			if (i == l.length() - 1) {
				if (! word.empty()) {
					PRINTF(("%s - %d", word.c_str(), isAbba(word)));

					if (ishypernet && isAbba(word)) {
						abbaInHypernet = true;
					}

					if (! ishypernet && isAbba(word)) {
						noAbbaBeforeSquare = false;
					}
				}
			}
		}

		if (! abbaInHypernet && ! noAbbaBeforeSquare) {
			partA++;
		}
	}

	PRINTF(("PART_A: %d", partA));
}
