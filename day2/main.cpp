#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::vector<int> codes = utils::string2Int(utils::strTok(File::readAllLines(argv[1])[0], ','));

	IntCodeMachine machine;

	{
		std::vector<int> program = codes;

		program[1] = 12;
		program[2] = 2;

		machine.run(program);

		PRINTF(("PART A: %d", program[0]));
	}

	{
		int expectedResult = atoi(argv[2]);

		for (int noun = 0; noun < 99; noun++) {
			for (int verb = 0; verb < 99; verb++) {
				std::vector<int> program = codes;

				program[1] = noun;
				program[2] = verb;

				machine.run(program);

				if (program[0] == expectedResult) {
					PRINTF(("Part B: %d", 100 * noun + verb));
					exit (1);
				}
			}
		}
	}
}
