#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


static bool isCorrectTriangle(int val[3]) {
	return \
		(val[0] + val[1] > val[2]) &&
		(val[1] + val[2] > val[0]) &&
		(val[2] + val[0] > val[1]);
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	int edges[data.size()][3];

	{
		int idx  = 0;
		int line = 0;

		for (auto &l : data) {
			if (l.empty()) {
				continue;
			}

			auto values  = utils::strTok(l, ' ');

			idx = 0;
			for (auto &v : values) {
				if (! v.empty()) {
					edges[line][idx++] = utils::toInt(v);
				}
			}
			line++;
		}
	}

	{
		int sum = 0;

		for (int i = 0; i < data.size(); i++) {
			if (isCorrectTriangle(edges[i])) {
				sum++;
			}
		}

		PRINTF(("PART_A: %d", sum));
	}

	{
		int sum = 0;

		int idx = 0;
		int val[3];
		for (int i = 0; i < data.size() * 3; i++) {
			int column = i / data.size();
			int row    = i % data.size();

			val[idx++] = edges[row][column];

			if (idx == 3) {
				if (isCorrectTriangle(val)) {
					sum++;
				}

				idx = 0;
			}
		}

		PRINTF(("PART_B: %d", sum));
	}
}
