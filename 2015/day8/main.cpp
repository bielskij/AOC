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


static int encodedLength(const std::string &str) {
	int ret = 2;

	for (int i = 0; i < str.length(); i++) {
		switch (str[i]) {
			case '"':
			case '\\':
				ret += 2;
				break;

			default:
				ret += 1;
		}
	}

	return ret;
}


static int decodedLength(const std::string &str) {
	int ret = 0;

	if (str.length() > 0) {
		int start = 0;
		int end   = 0;

		if (*str.begin() == '"' && *str.rbegin() == '"') {
			start = 1;
			end   = str.length() - 1;
		}

		for (int i = start; i < end; i++) {
			if (str[i] == '\\') {
				if (str[i + 1] == 'x') {
					ret += 1;
					i   += 3;
				} else {
					ret += 1;
					i   += 1;
				}

			} else {
				ret += 1;
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	{
		int totalBytes  = 0;
		int totalChars = 0;

		for (auto i = data.begin(); i != data.end(); i++) {
			totalBytes += i->length();
			totalChars += decodedLength(*i);
		}

		PRINTF(("PART_A: %d", totalBytes - totalChars));
	}

	{
		int totalBytes  = 0;
		int totalChars = 0;

		for (auto i = data.begin(); i != data.end(); i++) {
			totalBytes += i->length();
			totalChars += encodedLength(*i);
		}

		PRINTF(("PART_B: %d", totalChars - totalBytes));
	}
}
