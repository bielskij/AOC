#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


static uint32_t _calc(const std::string &str, int off) {
	uint32_t ret = 0;

	int posPrev = str.length() - off;

	for (int posCurr = 0; posCurr < str.length(); posCurr++) {
		if (str[posPrev] == str[posCurr]) {
			ret += (str[posPrev] - '0');
		}

		posPrev = (posPrev + 1) % str.length();
	}

	return ret;
}


static uint32_t _getResultA(const std::string &str) {
	return _calc(str, 1);
}


static uint32_t _getResultB(const std::string &str) {
	return _calc(str, str.length() / 2);
}


int main(int argc, char *argv[]) {
	std::string str = File::readAllLines(argv[1])[0];

	ASSERT(_getResultA("1122")     == 3);
	ASSERT(_getResultA("1111")     == 4);
	ASSERT(_getResultA("1234")     == 0);
	ASSERT(_getResultA("91212129") == 9);

	PRINTF(("PART_A: %u", _getResultA(str)));

	ASSERT(_getResultB("1212")     == 6);
	ASSERT(_getResultB("1221")     == 0);
	ASSERT(_getResultB("123425")   == 4);
	ASSERT(_getResultB("123123")   == 12);
	ASSERT(_getResultB("12131415") == 4);

	PRINTF(("PART_B: %u", _getResultB(str)));
}
