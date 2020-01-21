#include <stdexcept>
#include <climits>
#include <set>
#include <algorithm>

#include <openssl/md5.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static char _getLetter(const std::string &prefix, int &idx) {
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

	char ret = (mdBuffer[2] & 0x0f);
	if (ret > 9) {
		ret = ret - 10 + 'a';
	} else {
		ret = ret + '0';
	}

	return ret;
}


int main(int argc, char *argv[]) {
	std::string password;

	int idx = 0;
	for (int i = 0; i < 8; i++) {
		password.push_back(_getLetter(argv[1], idx));

		idx++;
	}

	PRINTF(("PART_A: %s", password.c_str()));
}
