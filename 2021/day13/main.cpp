#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<int, std::map<int, char>> paper;
		std::vector<std::string> rules;

		int paperWidth  = 0;
		int paperHeight = 0;

		{
			bool parsingRules = false;

			for (const auto &l : lines) {
				if (l.empty()) {
					parsingRules = true;

				} else {
					if (parsingRules) {
						rules.push_back(l);

					} else {
						int x, y;

						if (sscanf(l.c_str(), "%d,%d", &x, &y) == 2) {
							paper[y][x] = '#';

							if ((y + 1) > paperHeight) {
								paperHeight = y + 1;
							}

							if ((x + 1) > paperWidth) {
								paperWidth = x + 1;
							}

						} else {
							PRINTF(("Unable to parse dot! (%s)", l.c_str()));
							abort();
						}
					}
				}
			}
		}

		{
			int partA = 0;

			for (const auto &r : rules) {
				int val;

				if (sscanf(r.c_str(), "fold along x=%d", &val) == 1) {
					for (size_t x = 1; x <= (paperWidth / 2); x++) {
						for (int y = 0; y < paperHeight; y++) {
							if (paper[y][val - x] == '#' || paper[y][val + x] == '#') {
								paper[y][val - x] = '#';
							}
						}
					}

					paperWidth >>= 1;

				} else if (sscanf(r.c_str(), "fold along y=%d", &val) == 1) {
					for (size_t y = 1; y <= (paperHeight / 2); y++) {
						for (int x = 0; x < paperWidth; x++) {
							if (paper[val - y][x] == '#' || paper[val + y][x] == '#') {
								paper[val - y][x] = '#';
							}
						}
					}

					paperHeight >>= 1;

				} else {
					PRINTF(("Invalid rule!"));
					abort();
				}

				if (partA == 0) {
					for (int y = 0; y < paperHeight; y++) {
						auto &row = paper[y];

						for (int x = 0; x < paperWidth; x++) {
							if (row[x] == '#') {
								partA++;
							}
						}
					}

					PRINTF(("PART_A: %d", partA));
				}
			}
		}

		for (int y = 0; y < paperHeight; y++) {
			auto &row = paper[y];

			for (int x = 0; x < paperWidth; x++) {
				printf("%c", row[x] == '#' ? '#' : '.');
			}
			printf("\n");
		}
		printf("\n");
	}

	return 0;
}
