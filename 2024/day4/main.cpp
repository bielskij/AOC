#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static int calcXmas(const std::vector<std::string> &map) {
	int ret = 0;

	std::vector<std::array<int, 2>> modifiers = {
		{  1,  0 },
		{ -1,  0 },
		{  0,  1 },
		{  0, -1 },
		// diagonal
		{  1,  1 },
		{  1, -1 },
		{ -1,  1 },
		{ -1, -1 }
	};

	std::string xmas = "XMAS";

	for (int rowIdx = 0; rowIdx < map.size(); rowIdx++) {
		const auto &row = map[rowIdx];

		for (int colIdx = 0; colIdx < row.length(); colIdx++) {
			char c = row[colIdx];

			if (c == 'X') {
				for (const auto &mode : modifiers) {
					int matched = 0;

					for (int i = 1; i <= 3; i++) {
						int newX = colIdx + mode[0] * i;
						int newY = rowIdx + mode[1] * i;

						if (newX < row.length() && newY < map.size()) {
							if (map[newY][newX] == xmas[i]) {
								matched++;

							} else {
								break;
							}

						} else {
							break;
						}
					}

					if (matched == 3) {
						ret++;
					}
				}
			}
		}
	}

	return ret;
}

static int calcX_mas(const std::vector<std::string> &map) {
	int ret = 0;

	for (int rowIdx = 1; rowIdx < map.size() - 1; rowIdx++) {
		const auto &row = map[rowIdx];

		for (int colIdx = 1; colIdx < row.length() - 1; colIdx++) {
			auto c = row[colIdx];

			if (c == 'A') {
				if (
					(
						(
							(map[rowIdx - 1][colIdx - 1] == 'M') && 
							(map[rowIdx + 1][colIdx + 1] == 'S')
						) ||
						(
							(map[rowIdx - 1][colIdx - 1] == 'S') && 
							(map[rowIdx + 1][colIdx + 1] == 'M')
						)
					) &&
					(
						(
							(map[rowIdx - 1][colIdx + 1] == 'M') && 
							(map[rowIdx + 1][colIdx - 1] == 'S')
						) ||
						(
							(map[rowIdx - 1][colIdx + 1] == 'S') && 
							(map[rowIdx + 1][colIdx - 1] == 'M')
						)
					)
				) {
					ret++;
				}
			}
		}
	}

	return ret;
}

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", calcXmas(lines)));
	PRINTF(("PART_B: %d", calcX_mas(lines)));

	return 0;
}
