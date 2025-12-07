#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

    {
        auto data = lines;

        int partA = 0;

        
        for (int row = 1; row < data.size(); row++) {
            auto &prevRow = data[row - 1];
            auto &currRow = data[row];

            for (int col = 0; col < prevRow.length(); col++) {
                switch (prevRow[col]) {
                    case '^':
                        if (row > 1 && data[row - 2][col] == '|') {
                            partA++;

                            if (col - 1 >= 0) {
                                if (prevRow[col - 1] == '.') {
                                    prevRow[col - 1] = '|';
                                }
                            }

                            if (col + 1 < prevRow.length()) {
                                if (prevRow[col + 1] == '.') {
                                    prevRow[col + 1] = '|';
                                }
                            }
                        }
                        break;
                }
            }

            for (int col = 0; col < prevRow.length(); col++) {
                switch (prevRow[col]) {
                    case 'S':
                    case '|':
                        if (currRow[col] == '.') {
                            currRow[col] = '|';
                        }
                        break;
                }
            }
        }

        PRINTF(("PART_A: %d", partA));
    }

    {
        std::vector<uint64_t> beams;

        beams.resize(lines[0].size());

        beams[lines[0].find("S")] = 1;

        for (int row = 1; row < beams.size(); row++) {
            auto &line = lines[row];

            for (int col = 0; col < line.size(); col++) {
                if (line[col] == '^') {
                    if (col > 0) {
                        beams[col - 1] += beams[col];
                    }

                    if (col < beams.size() - 1) {
                        beams[col + 1] += beams[col];
                    }

                    beams[col] = 0;
                }
            }
        }

        uint64_t partB = 0;

        std::for_each(beams.begin(), beams.end(), [&partB](auto b) {
            partB += b;
        });

        PRINTF(("PART_B: %" PRIu64, partB));
    }

	return 0;
}
