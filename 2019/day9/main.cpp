#include <iostream>
#include <cstring>
#include <algorithm>
#include <queue>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class CustomIntMachine : public IntCodeMachine {
	public:
		CustomIntMachine(const std::vector<int64_t> &program) : IntCodeMachine(program) {

		}

		bool onOut(int64_t value) {
			PRINTF(("OUT> %" PRId64, value));

			return true;
		}

		bool onIn(int64_t &value) {
			std::string inputString;

			PRINTF(("IN<"));

			std::getline(std::cin, inputString);

			value = atoi(inputString.c_str());

			return true;
		}
};


int main(int argc, char *argv[]) {
	CustomIntMachine(utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','))).run();
}
