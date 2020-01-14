#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Computer {
	unsigned int reg[2];
	int pc;

	void reset() {
		this->pc     = 0;
		this->reg[0] = 0;
		this->reg[1] = 0;
	}

	Computer() {
		this->reg[0] = 0;
		this->reg[1] = 0;

		this->pc = 0;
	}

	int run(const std::vector<std::string> &code) {
		while (this->pc < code.size()) {
			const std::string &opcode = code[this->pc];

			if (opcode.compare(0, 3, "hlf") == 0) {
				this->reg[opcode[4] - 'a'] >>= 1;
				this->pc++;

			} else if (opcode.compare(0, 3, "tpl") == 0) {
				this->reg[opcode[4] - 'a'] *= 3;
				this->pc++;

			} else if (opcode.compare(0, 3, "inc") == 0) {
				this->reg[opcode[4] - 'a']++;
				this->pc++;

			} else if (opcode.compare(0, 3, "jmp") == 0) {
				this->pc += utils::toInt(opcode.substr(4));

			} else if (opcode.compare(0, 3, "jie") == 0) {
				if ((this->reg[opcode[4] - 'a'] & 0x01) == 0) {
					this->pc += utils::toInt(opcode.substr(7));

				} else {
					this->pc++;
				}

			} else if (opcode.compare(0, 3, "jio") == 0) {
				if (this->reg[opcode[4] - 'a'] == 1) {
					this->pc += utils::toInt(opcode.substr(7));

				} else {
					this->pc++;
				}
			} else {
				abort();
			}
		}

		return this->reg[1];
	}
};


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	{
		Computer c;

		PRINTF(("PART_A: %d", c.run(data)));

		c.reset();
		c.reg[0] = 1;

		PRINTF(("PART_B: %d", c.run(data)));
	}
}
