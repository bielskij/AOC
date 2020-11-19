#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#include "computer.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	{
		Computer c;

		PRINTF(("PART_A: %d", c.run(data)));

		c.reset();
		c.reg[Computer::toReg("c")] = 1;

		PRINTF(("PART_B: %d", c.run(data)));
	}
}
