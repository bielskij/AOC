#include <iostream>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#include "common/debug.h"


class CustomIntMachine : public IntCodeMachine {
	public:
		CustomIntMachine(const std::vector<int> &program) : IntCodeMachine(program) {

		}

		bool onOut(int value) {
			PRINTF(("OUT> %d", value));

			return true;
		}

		bool onIn(int &value) {
			std::string inputString;

			PRINTF(("IN<"));

			std::getline(std::cin, inputString);

			value = atoi(inputString.c_str());

			return true;
		}
};


int main(int argc, char *argv[]) {
	CustomIntMachine(utils::string2Int(utils::strTok(File::readAllLines(argv[1])[0], ','))).run();
}
