#include <assert.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int64_t modulus = 20201227;


int getLoopSize(int key, int subjectNumber) {
	int ret = 0;

	{
		int64_t value = 1;

		while (value != key) {
			value *= subjectNumber;
			value %= modulus;

			ret++;
		}
	}

	return ret;
}


static int getEncryptionKey(int publicKey, int loopSize) {
	int64_t ret = 1;

	while (loopSize-- > 0) {
		ret *= publicKey;
		ret %= modulus;
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int publicKey[2] = {
			utils::toInt(lines[0]),
			utils::toInt(lines[1]),
		};

		int loopSize[2];
		int encKey[2];

		loopSize[0] = getLoopSize(publicKey[0], 7);
		loopSize[1] = getLoopSize(publicKey[1], 7);

		encKey[0] = getEncryptionKey(publicKey[0], loopSize[1]);
		encKey[1] = getEncryptionKey(publicKey[1], loopSize[0]);

		assert(encKey[0] == encKey[1]);

		PRINTF(("PART_A: %d", encKey[0]));
	}

	return 0;
}
