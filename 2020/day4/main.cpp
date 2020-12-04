#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	int partA = 0;
	int partB = 0;
	std::map<std::string, std::string> passport;

	for (auto lineIt = lines.begin(); lineIt != lines.end(); lineIt++) {
		std::string &line = *lineIt;

		if (! line.empty()) {
			auto tokens = utils::strTok(line, ' ');

			for (auto token : tokens) {
				auto keyVal = utils::strTok(token, ':');

				passport[keyVal[0]] = keyVal[1];
			}
		}

		if (line.empty() || lineIt + 1 == lines.end()) {
			int validKeys = 0;

			if (passport.find("byr") != passport.end()) validKeys++;
			if (passport.find("iyr") != passport.end()) validKeys++;
			if (passport.find("eyr") != passport.end()) validKeys++;
			if (passport.find("hgt") != passport.end()) validKeys++;
			if (passport.find("hcl") != passport.end()) validKeys++;
			if (passport.find("ecl") != passport.end()) validKeys++;
			if (passport.find("pid") != passport.end()) validKeys++;
			if (passport.find("cid") != passport.end()) validKeys++;

			{
				bool valid = false;

				if (validKeys == 8) {
					valid = true;

				} else if (validKeys == 7) {
					if (passport.find("cid") == passport.end()) {
						valid = true;
					}
				}

				if (valid) {
					partA++;

					if (valid) {
						int byr = utils::toInt(passport["byr"]);
						if (byr < 1920 || byr > 2002) {
							valid = false;
						}
					}

					if (valid) {
						int byr = utils::toInt(passport["iyr"]);
						if (byr < 2010 || byr > 2020) {
							valid = false;
						}
					}

					if (valid) {
						int byr = utils::toInt(passport["eyr"]);
						if (byr < 2020 || byr > 2030) {
							valid = false;
						}
					}

					if (valid) {
						const std::string &h = passport["hgt"];
						std::string units = h.substr(h.length() - 2, 2);
						int val = utils::toInt(h.substr(0, h.length() - 2));

						if (units == "cm") {
							if (val < 150 || val > 193) {
								valid = false;
							}
						} else if (units == "in") {
							if (val < 59 || val > 76) {
								valid = false;
							}

						} else {
							valid = false;
						}
					}

					if (valid) {
						const std::string &h = passport["hcl"];

						if (h[0] != '#') {
							valid = false;

						} else {
							valid = utils::isHex(h.substr(1));
						}
					}

					if (valid) {
						const std::string &v = passport["ecl"];

						if (v != "amb" && v != "blu" && v != "brn" && v != "gry" && v != "grn" && v != "hzl" && v != "oth") {
							valid = false;
						}
					}

					if (valid) {
						const std::string &v = passport["pid"];

						if (v.length() != 9 || ! utils::isNumber(v)) {
							valid = false;
						}
					}

					if (valid) {
						partB++;
					}
				}
			}

			passport.clear();

			continue;
		}
	}

	PRINTF(("PART_A: %zd", partA));
	PRINTF(("PART_B: %d",  partB));
}
