#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#include "computer.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static bool run(const std::vector<std::string> &code, int &acc) {
	bool ret = false;

	{
		Computer computer;

		std::set<int> pcs;

		while (pcs.find(computer.pc()) == pcs.end()) {
			pcs.insert(computer.pc());

			computer.tick(code);

			if (computer.pc() >= code.size()) {
				ret = true;
				break;
			}
		}

		acc = computer.acc();
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int acc;

		run(lines, acc);

		PRINTF(("PART_A: %d", acc));
	}

	{
		std::vector<std::string> code = lines;

		for (auto &l : code) {
			int acc;

			if (l.compare(0, 3, "nop") == 0) {
				l.replace(0, 3, "jmp");

			} else if (l.compare(0, 3, "jmp") == 0) {
				l.replace(0, 3, "nop");

			} else {
				continue;
			}

			if (run(code, acc)) {
				PRINTF(("PART_B: %d", acc));
				return 0;
			}

			code = lines;
		}
	}
}
