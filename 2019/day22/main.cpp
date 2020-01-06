#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <unistd.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


typedef __int128 int128_t;


enum ShufflingTechnique {
	CUT,
	DEAL_NEW_STACK,
	DEAL_INCREMENT
};


static int64_t getPartA(std::vector<std::pair<ShufflingTechnique, int64_t>> &techniques, int64_t max, int64_t value) {
	for (auto &t : techniques) {
		switch (t.first) {
			case ShufflingTechnique::CUT:
				{
					value = (max - t.second + value) % max;
				}
				break;

			case ShufflingTechnique::DEAL_INCREMENT:
				{
					value = (value * t.second) % max;
				}
				break;

			case ShufflingTechnique::DEAL_NEW_STACK:
				{
					value = max - value - 1;
				}
				break;
		}
	}

	return value;
}


int128_t _mod(int128_t a, int128_t b) {
	if (a >= 0) {
		return a % b;

	} else {
		return b + a % b;
	}
}

// Modulus inversion
int128_t _inv(int128_t b, int128_t n) {
	int128_t x, y;
	int128_t g = utils::gcd_ext<int128_t>(b, n, x, y);

	return (g != 1) ? -1 : _mod(x, n);
}


int128_t _pow_mod(int128_t base, int128_t exponent, int128_t n) {
	if (exponent == 0) {
		return (base == 0) ? 0 : 1;
	}

	int128_t bit = 1;
	int128_t power = _mod(base, n);
	int128_t out = 1;

	while (bit <= exponent) {
		if (exponent & bit) {
			out = _mod(out * power, n);
		}

		power = _mod(power * power, n);
		bit <<= 1;
	}

	return out;
}


int128_t _div_mod(int128_t a, int128_t b, int128_t n) {
	int128_t inv = _inv(b, n);

	a = _mod(a, n);

	return (inv == -1) ? -1 : (a * inv) % n;
}


static int64_t getPartB(std::vector<std::pair<ShufflingTechnique, int64_t>> &techniques, int128_t c, int128_t n, int128_t position) {
	int128_t a = 1;
	int128_t b = 0;

	for (auto it = techniques.begin(); it != techniques.end(); it++) {
		int128_t tmpA;
		int128_t tmpB;

		switch (it->first) {
			case ShufflingTechnique::CUT:
				{
					if (it->second < 0) {
						it->second += n;
					}

					tmpA = 1;
					tmpB = n - it->second;
				}
				break;

			case ShufflingTechnique::DEAL_INCREMENT:
				{
					tmpA = it->second;
					tmpB = 0;
				}
				break;

			case ShufflingTechnique::DEAL_NEW_STACK:
				{
					tmpA = -1;
					tmpB = n - 1;
				}
				break;
		}

		a = _mod(tmpA * a, n);
		b = _mod(tmpA * b + tmpB, n);
	}

	int128_t retA = _pow_mod(a, c, n);
	int128_t retB = _mod(b * _div_mod(_pow_mod(a, c, n) - 1, a - 1, n), n);

	return _mod(_div_mod(_mod(position - retB, n), retA, n), n);
}


int main(int argc, char *argv[]) {
	auto rules = File::readAllLines(argv[1]);

	{
		std::vector<std::pair<ShufflingTechnique, int64_t>> techniques;

		for (int i = 0; i < rules.size(); i++) {
			int value;

			if (sscanf(rules[i].c_str(), "cut %d", &value) == 1) {
				techniques.push_back(std::pair<ShufflingTechnique, int64_t>(ShufflingTechnique::CUT, value));

			} else if (sscanf(rules[i].c_str(), "deal with increment %d", &value) == 1) {
				techniques.push_back(std::pair<ShufflingTechnique, int64_t>(ShufflingTechnique::DEAL_INCREMENT, value));

			} else if (rules[i].compare("deal into new stack") == 0) {
				techniques.push_back(std::pair<ShufflingTechnique, int64_t>(ShufflingTechnique::DEAL_NEW_STACK, 0));
			}
		}

		PRINTF(("PART_A: %" PRId64, getPartA(techniques, 10007, 2019)));
		PRINTF(("PART_B: %" PRId64, getPartB(techniques, 101741582076661, 119315717514047, 2020)));
	}
}
