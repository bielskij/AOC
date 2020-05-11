#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::string src      = argv[1];
	int         diskSize = utils::toInt(argv[2]);

	std::string a = src;
	std::string b;

	bool finished = false;

	while (! finished) {
		b = a;

		std::reverse(b.begin(), b.end());

		for (auto &c : b) {
			c = (c == '0') ? '1' : '0';
		}

		a = a + "0" + b;

		if (a.length() >= diskSize) {
			std::string checksum = a.substr(0, diskSize);

			do {
				std::string tmp;

				for (int i = 0; i < checksum.length(); i += 2) {
					if (checksum[i] == checksum[i + 1]) {
						tmp.push_back('1');

					} else {
						tmp.push_back('0');
					}
				}

				checksum = tmp;

			} while (checksum.size() > 0 && (checksum.size() & 1) == 0);

			PRINTF(("RESULT: '%s'", checksum.c_str()));

			finished = true;
		}
	}
}
