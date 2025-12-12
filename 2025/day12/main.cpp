#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

struct Puzzle {
	std::vector<std::vector<bool>> pixels;

	Puzzle() {
		this->activePixels = 0;
	}

	void putRow(const std::string &row) {
		std::vector<bool> rowPixels;

		for (auto c : row) {
			if (c == '#') {
				this->activePixels++;

				rowPixels.push_back(true);

			} else {
				rowPixels.push_back(false);
			}
		}

		this->pixels.push_back(rowPixels);
	}

	int getAreaSize() const {
		if (this->pixels.empty()) {
			return 0;
		}

		return this->pixels.size() * this->pixels[0].size();
	}

	int getActivePixels() const {
		return this->activePixels;
	}

	int activePixels;
};

struct Region {
	int width;
	int height;

	std::vector<int> quantityList;

	Region() : quantityList(6, 0) {
	}

	int getSize() const {
		return this->width * this->height;
	}
};

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<Puzzle> puzzle;
	std::vector<Region> regions;

	{
		bool parsePuzzles = true;

		int puzzleId = 0;

		for (const auto &line : lines) {
			if (line.empty()) {
				continue;
			}

			if (parsePuzzles) {
				if (line.length() == 2) {
					puzzleId = std::stoi(line.substr(0, line.length() - 1));
					if (puzzleId + 1 > puzzle.size()) {
						puzzle.push_back(Puzzle());
					}

					continue;
				}

				if (line.length() == 3) {
					puzzle.rbegin()->putRow(line);

				} else {
					parsePuzzles = false;
				}
			}

			if (! parsePuzzles) {
				Region region;

				if (sscanf(line.c_str(), "%dx%d: %d %d %d %d %d %d", 
					&region.width,
					&region.height,
					&region.quantityList.data()[0],
					&region.quantityList.data()[1],
					&region.quantityList.data()[2],
					&region.quantityList.data()[3],
					&region.quantityList.data()[4],
					&region.quantityList.data()[5]
				) != 8) {
					abort();
				}

				regions.emplace_back(region);
			}
		}
	}

	{
		int partA = 0;

		for (const auto &r : regions) {
			int minOccupiedArea = 0;
			int maxOccupiedArea = 0;

			for (int i = 0; i < r.quantityList.size(); i++) {
				minOccupiedArea += puzzle[i].getActivePixels() * r.quantityList[i];
				maxOccupiedArea += puzzle[i].getAreaSize()     * r.quantityList[i];
			}

			if (r.getSize() >= maxOccupiedArea) {
				// Fit in any configuration
				partA++;

			} else if (r.getSize() < minOccupiedArea) {
				// Cannot fit

			} else {
				// Need complex analysis
				abort();
			}
		}

		PRINTF(("PART_A: %d", partA));
	}

	return 0;
}
