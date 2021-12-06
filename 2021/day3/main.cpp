#include <functional>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int _calcPartB(const std::vector<std::string> &samples, std::function<void(std::vector<std::string> &, const std::vector<std::string> &, const std::vector<std::string> &)> cond) {
	std::vector<std::string> v = samples;

	int bitPos = 0;

	while (v.size() > 1 && bitPos < v[0].length()) {
		std::vector<std::string> ones;
		std::vector<std::string> zeroes;

		for (const auto &num : v) {
			if (num[bitPos] == '0') {
				zeroes.push_back(num);

			} else {
				ones.push_back(num);
			}
		}

		cond(v, zeroes, ones);

		bitPos++;
	}

	return strtol(v.begin()->c_str(), NULL, 2);
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int gamma   = 0;
		int epsilon = 0;

		for (int i = 0; i < lines[0].length(); i++) {
			int ones   = 0;
			int zeroes = 0;

			gamma   <<= 1;
			epsilon <<= 1;

			for (auto &number : lines) {
				if (number[i] == '0') {
					zeroes++;

				} else {
					ones++;
				}
			}

			if (ones > zeroes) {
				gamma   |= 1;
			} else {
				epsilon |= 1;
			}
		}

		PRINTF(("PART_A: %d", gamma * epsilon));
	}

	{
		int partB = 0;

		int oxygen = _calcPartB(lines, [](std::vector<std::string> &result, const std::vector<std::string> &zeroes, const std::vector<std::string> &ones) {
			if (ones.size() >= zeroes.size()) {
				result = ones;

			} else {
				result = zeroes;
			}
		});

		int co2 = _calcPartB(lines, [](std::vector<std::string> &result, const std::vector<std::string> &zeroes, const std::vector<std::string> &ones) {
			if (zeroes.size() <= ones.size()) {
				result = zeroes;

			} else {
				result = ones;
			}
		});

		PRINTF(("PART_B: %d (%d, %d)", oxygen * co2, oxygen, co2));
	}

	return 0;
}
