#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#include "common/debug.h"


int main(int argc, char *argv[]) {
	IntCodeMachine machine(utils::string2Int64t(utils::strTok(File::readAllLines(argv[1])[0], ',')));

	{
		machine.getMemory()[1] = 12;
		machine.getMemory()[2] = 2;

		machine.run();

		PRINTF(("PART A: %" PRId64, machine.getMemory()[0]));
	}

	{
		int expectedResult = atoi(argv[2]);

		for (int noun = 0; noun < 99; noun++) {
			for (int verb = 0; verb < 99; verb++) {
				machine.reset();

				machine.getMemory()[1] = noun;
				machine.getMemory()[2] = verb;

				machine.run();

				if (machine.getMemory()[0] == expectedResult) {
					PRINTF(("Part B: %d", 100 * noun + verb));
					exit (1);
				}
			}
		}
	}
}
