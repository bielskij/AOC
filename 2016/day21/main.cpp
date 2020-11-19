#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static std::string _rotate(const std::string str, int val, bool right) {
	if (val == 0) {
		return str;

	} else {
		std::string ret = str;

		val = val % str.size();

		for (int i = 0; i < str.size(); i++) {
			int srcPos = right ? (str.size() - val + i) : (val + i);

			ret[i] = str[srcPos % str.size()];
		}

		return ret;
	}
}


static int _revMod(int div, int a, int rem) {
	if (rem >= div) {
		ERR(("-->"));
		abort();
	}

	if (a < rem) {
		return rem - a;
	}

	return div + rem - a;
}


std::string _do(const std::vector<std::string> &_rules, const std::string &src, bool reverse) {
	std::string dst = src;
	std::string tmp;

	auto rules = reverse ? std::vector<std::string>(_rules.rbegin(), _rules.rend()) : _rules;

	auto it = rules.begin();

	while (it != rules.end()) {
		int intArg1, intArg2;
		char charArg1, charArg2;

		tmp = dst;

		if (sscanf(it->c_str(), "swap position %d with position %d", &intArg1, &intArg2) == 2) {
			charArg1 = tmp[intArg1];

			tmp[intArg1] = tmp[intArg2];
			tmp[intArg2] = charArg1;

		} else if (sscanf(it->c_str(), "swap letter %c with letter %c", &charArg1, &charArg2) == 2) {
			for (int i = 0; i < dst.size(); i++) {
				if (dst[i] == charArg1) {
					tmp[i] = charArg2;

				} else if (dst[i] == charArg2) {
					tmp[i] = charArg1;
				}
			}

		} else if (sscanf(it->c_str(), "rotate right %d steps", &intArg1) == 1) {
			tmp = _rotate(dst, intArg1, ! reverse);

		} else if (sscanf(it->c_str(), "rotate left %d steps", &intArg1) == 1) {
			tmp = _rotate(dst, intArg1, reverse);

		} else if (sscanf(it->c_str(), "rotate based on position of letter %c", &charArg1) == 1) {
			int letterPos = dst.find(charArg1);

			if (letterPos != std::string::npos) {
				int pos;

				if (! reverse) {
					pos = 1 + letterPos;

					if (letterPos >= 4) {
						pos++;
					}

				} else {
					bool greaterThanFour = false;

					pos = _revMod(dst.size(), 1, letterPos) % dst.size();
					if (pos & 1) {
						pos -= 1;

						greaterThanFour = true;
					}

					pos /= 2;

					if (greaterThanFour) {
						pos += 4;
					}

					if (letterPos > pos) {
						pos = letterPos - pos;

					} else {
						pos = letterPos + (dst.size() - pos);
					}
				}

				tmp = _rotate(dst, pos, ! reverse);
			}

		} else if (sscanf(it->c_str(), "reverse positions %d through %d", &intArg1, &intArg2) == 2) {
			for (int i = 0; i <= intArg2 - intArg1; i++) {
				tmp[intArg1 + i] = dst[intArg2 - i];
			}

		} else if (sscanf(it->c_str(), "move position %d to position %d", &intArg1, &intArg2) == 2) {
			char srcPos = reverse ? intArg2 : intArg1;
			char dstPos = reverse ? intArg1 : intArg2;

			charArg1 = dst[srcPos];

			tmp.erase(srcPos, 1);
			tmp.insert(dstPos, 1, charArg1);

		} else {
			ERR(("Unknown rule '%s'",  it->c_str()));
			abort();
		}

//		DBG(("DST (%s) -< TMP (%s)", dst.c_str(), tmp.c_str()));

		dst = tmp;

		it++;
	}

	return dst;
}


int main(int argc, char *argv[]) {
	auto rules     = File::readAllLines(argv[1]);
	auto plain     = std::string(argv[2]);
	auto scrambled = std::string(argv[3]);

	{
		std::string dst;

		dst = _do(rules, plain, false);
		PRINTF(("PART_A: '%s'", dst.c_str()));

		dst = _do(rules, scrambled, true);
		PRINTF(("PART_B: '%s'", dst.c_str()));
	}
}
