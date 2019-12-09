#include <stdexcept>
#include <climits>
#include <set>
#include <algorithm>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::vector<std::string> data = File::readAllLines(argv[1]);

	{
		long long int sum = 0;

		for (auto row = data.begin(); row != data.end(); row++) {
			std::vector<std::string> numbers = utils::strTok(*row, 'x');

			int l = utils::toInt(numbers[0]);
			int w = utils::toInt(numbers[1]);
			int h = utils::toInt(numbers[2]);

			std::vector<int> squares;

			squares.push_back(l * w);
			squares.push_back(w * h);
			squares.push_back(h * l);

			sum += (2 * squares[0] + 2 * squares[1] + 2 * squares[2] + *std::min_element(squares.begin(), squares.end()));
		}

		PRINTF(("PART A: %lld", sum));
	}

	{
		long long int sum = 0;

		for (auto row = data.begin(); row != data.end(); row++) {
			std::vector<std::string> numbers = utils::strTok(*row, 'x');

			std::vector<int> dimm = { utils::toInt(numbers[0]), utils::toInt(numbers[1]), utils::toInt(numbers[2]) };

			std::sort(dimm.begin(), dimm.end());

			sum += (2 * dimm[0] + 2 * dimm[1] + dimm[0] * dimm[1] * dimm[2]);
		}

		PRINTF(("PART B: %lld", sum));
	}
}
