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


static int _getNumber(char *word, int zeros) {
	int number = 0;

	char buffer[128];
	int bufferWritten;

	unsigned char mdBuffer[MD5_DIGEST_LENGTH];

	do {
		bufferWritten = snprintf(buffer, sizeof(buffer), "%s%d", word, number++);

		MD5((unsigned char *) buffer, bufferWritten, mdBuffer);

		if (
			(mdBuffer[0] == 0) &&
			(mdBuffer[1] == 0)
		) {
			if (zeros == 5) {
				if (mdBuffer[2] >> 4 == 0) {
					break;
				}

			} else if (mdBuffer[2] == 0) {
				break;
			}
		}
	} while (1);

	return number - 1;
}


int main(int argc, char *argv[]) {
	PRINTF(("PART A: %d", _getNumber(argv[1], 5)));
	PRINTF(("PART B: %d", _getNumber(argv[1], 6)));
}
