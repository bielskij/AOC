#include <iostream>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#include "common/debug.h"


class CustomIntMachine : public IntCodeMachine {
	public:
		void onOut(int value) {
			PRINTF(("OUT> %d", value));
		}

		int  onIn() {
			std::string inputString;

			PRINTF(("IN<"));

			std::getline(std::cin, inputString);

			return atoi(inputString.c_str());
		}
};


int main(int argc, char *argv[]) {
	std::vector<int> codes = utils::string2Int(utils::strTok(File::readAllLines(argv[1])[0], ','));

	CustomIntMachine machine;

	machine.run(codes);
}
