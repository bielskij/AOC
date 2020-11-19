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

		c.reg[0] = 7;

		PRINTF(("PART_A: %d", c.run(data)));

		c.reset();
		c.reg[0] = 12;

		PRINTF(("PART_B: %d", c.run(data)));
	}
}
