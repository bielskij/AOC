#include <cstring>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<std::string, int> regs;

		int partB = std::numeric_limits<int>::min();

		for (const auto &l : lines) {
			char regOp[32];
			char regCond[32];
			char op[8];
			char cond[8];
			int  mod;
			int  condVal;

			if (sscanf(l.c_str(), "%s %s %d if %s %s %d", regOp, op, &mod, regCond, cond, &condVal) != 6) {
				PRINTF(("Scanf failed!"));
				abort();

			} else {
				bool condMet = false;

				if (strcmp(cond, ">") == 0) {
					condMet = regs[regCond] > condVal;

				} else if (strcmp(cond, "<") == 0) {
					condMet = regs[regCond] < condVal;

				} else if (strcmp(cond, ">=") == 0) {
					condMet = regs[regCond] >= condVal;

				} else if (strcmp(cond, "<=") == 0) {
					condMet = regs[regCond] <= condVal;

				} else if (strcmp(cond, "!=") == 0) {
					condMet = regs[regCond] != condVal;

				} else if (strcmp(cond, "==") == 0) {
					condMet = regs[regCond] == condVal;

				} else {
					PRINTF(("Unsupported condition!"));
					abort();
				}

				if (condMet) {
					if (strcmp(op, "inc") == 0) {
						regs[regOp] += mod;

					} else if (strcmp(op, "dec") == 0) {
						regs[regOp] -= mod;

					} else {
						PRINTF(("Unsupported operation!"));
						abort();
					}

					if (regs[regOp] > partB) {
						partB = regs[regOp];
					}
				}
			}
		}

		{
			int max = std::numeric_limits<int>::min();

			for (const auto &p : regs) {
				if (p.second > max) {
					max = p.second;
				}
			}

			PRINTF(("PART_A: %d", max));
		}

		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
