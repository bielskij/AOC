#include <stdexcept>
#include <climits>
#include <cstring>
#include <set>
#include <algorithm>

#include <openssl/md5.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


char bin2char(char v) {
	if (v > 9) {
		return v - 10 + 'a';

	} else {
		return v + '0';
	}
}


char char2bin(char v) {
	if (v >= 'a') {
		return 10 + (v - 'a');

	} else {
		return v - '0';
	}
}


static std::pair<char, char> _getLetter(const std::string &prefix, int &idx) {
	char buffer[128];
	int bufferWritten;

	unsigned char mdBuffer[MD5_DIGEST_LENGTH];

	do {
		bufferWritten = snprintf(buffer, sizeof(buffer), "%s%d", prefix.c_str(), idx);

		MD5((unsigned char *) buffer, bufferWritten, mdBuffer);

		if (
			(mdBuffer[0] == 0) &&
			(mdBuffer[1] == 0)
		) {
			if (mdBuffer[2] >> 4 == 0) {
				break;
			}
		}

		idx++;
	} while (1);

	std::pair<char, char> ret;

	ret.first  = bin2char (mdBuffer[2] & 0x0f);
	ret.second = bin2char((mdBuffer[3] & 0xf0) >> 4);

	return ret;
}


int main(int argc, char *argv[]) {
	{
		std::string partA;

		int idx = 0;
		for (int i = 0; i < 8; i++) {
			auto val = _getLetter(argv[1], idx);

			partA.push_back(val.first);

			idx++;
		}

		PRINTF(("PART_A: %s", partA.c_str()));
	}

	{
		char partB[8];

		memset(partB, -1, sizeof(partB));

		int idx = 0;
		while (true) {
			bool anyUnknown = false;

			for (int i = 0; i < 8; i++) {
				printf("%c", partB[i] == -1 ? '_' : partB[i]);

				if (partB[i] == -1) {
					anyUnknown = true;
				}
			}
			printf("\n");

			if (! anyUnknown) {
				break;
			}

			auto val = _getLetter(argv[1], idx);
			char bin = char2bin(val.first);
			if (bin < 8) {
				if (partB[bin] == -1) {
					partB[bin] = val.second;
				}
			}

			idx++;
		}

		PRINTF(("PART_B: %s", std::string(partB, sizeof(partB)).c_str()));
	}
}
