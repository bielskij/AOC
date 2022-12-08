#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int partA = 2 * lines.size() + (lines[0].length() - 2) * 2;
		int partB = 0;

		int height = lines.size();
		int width  = lines[0].length();

		for (size_t x = 1; x < width - 1; x++) {
			for (size_t y = 1; y < height - 1; y++) {

				int directions = 4;
				int distance;

				std::vector<int> distances;

				distance = 0;
				for (size_t xx = x + 1; xx < width; xx++) {
					distance++;
					if (lines[y][x] <= lines[y][xx]) {
						directions--;
						break;
					}
				}
				distances.push_back(distance);

				distance = 0;
				for (ssize_t xx = x - 1; xx >= 0; xx--) {
					distance++;
					if (lines[y][x] <= lines[y][xx]) {
						directions--;
						break;
					}
				}
				distances.push_back(distance);

				distance = 0;
				for (size_t yy = y + 1; yy < height; yy++) {
					distance++;
					if (lines[y][x] <= lines[yy][x]) {
						directions--;
						break;
					}
				}
				distances.push_back(distance);

				distance = 0;
				for (ssize_t yy = y - 1; yy >= 0; yy--) {
					distance++;
					if (lines[y][x] <= lines[yy][x]) {
						directions--;
						break;
					}
				}
				distances.push_back(distance);

				if (directions) {
					partA++;
				}

				{
					int scenicScore = 1;

					for (auto d : distances) {
						scenicScore *= d;
					}

					if (scenicScore > partB) {
						partB = scenicScore;
					}
				}
			}
		}

		PRINTF(("PART_A: %d", partA));
		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
