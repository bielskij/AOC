#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::string algorithm;
	std::vector<std::string> image;

	{
		bool parseAlgorithm = true;

		for (const auto &line : File::readAllLines(argv[1])) {
			if (line.empty()) {
				parseAlgorithm = false;

			} else {
				if (parseAlgorithm) {
					algorithm = line;

				} else {
					image.push_back(line);
				}
			}
		}
	}

	{
		std::vector<std::string> srcImage = image;
		std::vector<std::string> result;

		int padding = 0;

		int xMod[] = { -1,  0,  1, -1, 0, 1, -1, 0, 1 };
		int yMod[] = { -1, -1, -1,  0, 0, 0,  1, 1, 1 };

		for (int i = 0; i < 50; i++) {
			result.clear();

			if (padding != 3) {
				padding  = 3;

			} else {
				padding  = -1;
			}

			for (int row = 0; row < srcImage.size() + padding * 2; row++) {
				std::string outRow;

				for (int col = 0; col < srcImage[0].length() + padding * 2; col++) {
					int padRow = row - padding;
					int padCol = col - padding;

					int index  = 0;

					for (int idx = 0; idx < 9; idx++) {
						int cx = padCol + xMod[8 - idx];
						int cy = padRow + yMod[8 - idx];

						if (
							(cy >= 0) && (cy < srcImage.size()) &&
							(cx >= 0) && (cx < srcImage[0].length())
						) {
							index |= (((srcImage[cy][cx] == '#') ? 1 : 0) << idx);
						}
					}

					outRow.push_back(algorithm[index]);
				}

				result.push_back(outRow);
			}

			if ((i == 1) || (i == 49)) {
				int litNum = 0;

				for (const auto &r : result) {
					for (auto c : r) {
						if (c == '#') {
							litNum++;
						}
					}
				}

				PRINTF(("PART_%c: %d", i == 1 ? 'A' : 'B', litNum));
			}

			srcImage = result;
		}
#if 0
		for (const auto &r : result) {
			for (auto c : r) {
				printf("%c", c);
			}
			printf("\n");
		}
		printf("\n");
#endif
	}

	return 0;
}
