#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static const std::string snafuDigits = "=-012";


static int64_t snafu2Dec(const std::string &snafu) {
	int64_t ret = 0;

	for (auto c : snafu) {
		ret = 5 * ret + snafuDigits.find(c) - 2;
	}

	return ret;
}


static std::string dec2snafu(int64_t dec) {
	std::string ret;

	while (dec > 0) {
		auto snafuDigitIdx = (dec + 2) % 5;

		ret.push_back(snafuDigits[snafuDigitIdx]);

		dec += 2 - snafuDigitIdx;
		dec /= 5;
	}

	std::reverse(ret.begin(), ret.end());

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int64_t sum = 0;

		for (auto &l : lines) {
			sum += snafu2Dec(l);
		}

		PRINTF(("PART_A: '%s' (%ld)", dec2snafu(sum).c_str(), sum));
	}

	return 0;
}
