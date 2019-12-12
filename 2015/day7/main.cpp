#include <stdexcept>
#include <climits>
#include <cstdint>
#include <set>
#include <algorithm>
#include <map>

#include <openssl/md5.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static bool _getValue(const std::map<std::string, uint16_t> &pins, const std::string &pin, uint16_t &out) {
	bool ret = false;

	if (utils::isNumber(pin)) {
		out = utils::toInt(pin);
		ret = true;

	} else {
		auto it = pins.find(pin);
		if (it != pins.end()) {
			out = it->second;
			ret = true;
		}
	}

	return ret;
}


static void _calculateSignals(const std::vector<std::string> &_rules, std::map<std::string, uint16_t> &pins) {
	auto src = _rules;

	while (! src.empty()) {
		auto rule = src.begin();
		while (rule != src.end()) {
			auto words = utils::strTok(*rule, ' ');

			if (words[0].compare("NOT") == 0) {
				auto input = pins.find(words[1]);
				if (input != pins.end()) {
					pins[words[3]] = ~input->second;

					rule = src.erase(rule);

				} else {
					rule++;
				}

			} else {
				std::string &type = words[1];

				uint16_t inA, inB;

				if (type.compare("AND") == 0) {
					if (_getValue(pins, words[0], inA) && _getValue(pins, words[2], inB)) {
						pins[words[4]] = inA & inB;

						rule = src.erase(rule);

					} else {
						rule++;
					}

				} else if (type.compare("OR") == 0) {
					if (_getValue(pins, words[0], inA) && _getValue(pins, words[2], inB)) {
						pins[words[4]] = inA | inB;

						rule = src.erase(rule);

					} else {
						rule++;
					}

				} else if (type.compare("LSHIFT") == 0) {
					if (_getValue(pins, words[0], inA)) {
						pins[words[4]] = inA << utils::toInt(words[2]);

						rule = src.erase(rule);

					} else {
						rule++;
					}

				} else if (type.compare("RSHIFT") == 0) {
					if (_getValue(pins, words[0], inA)) {
						pins[words[4]] = inA >> utils::toInt(words[2]);

						rule = src.erase(rule);

					} else {
						rule++;
					}

				} else if (type.compare("->") == 0) {
					if (_getValue(pins, words[0], inA)) {
						pins[words[2]] = inA;

						rule = src.erase(rule);

					} else {
						rule++;
					}

				} else {
					PRINTF(("Not supported type! %s", type.c_str()));
					abort();
				}
			}
		}
	}
}


int main(int argc, char *argv[]) {
	auto rules = File::readAllLines(argv[1]);

	uint16_t alternateValue;

	{
		std::map<std::string, uint16_t> pins;

		_calculateSignals(rules, pins);

		alternateValue = pins["a"];

		PRINTF(("PART_A: %u", alternateValue));
	}

	{
		std::map<std::string, uint16_t> pins;

		for (auto rule = rules.begin(); rule != rules.end(); rule++) {
			auto words = utils::strTok(*rule, ' ');

			if (words[1].compare("->") == 0) {
				if (words[2].compare("b") == 0) {
					char buff[64];

					snprintf(buff, sizeof(buff), "%u -> b", alternateValue);

					rule->assign(buff);
				}
			}
		}

		_calculateSignals(rules, pins);

		PRINTF(("PART_B: %u", pins["a"]));
	}
}
