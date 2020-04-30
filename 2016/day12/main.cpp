#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Computer {
	long long int reg[4];
	int pc;

	void reset() {
		this->pc = 0;

		for (auto &r : reg) {
			r = 0;
		}
	}

	Computer() {
		this->reset();
	}

	static bool isReg(const std::string &str) {
		if (str.length() != 1) {
			return false;
		}

		if (str[0] >= 'a' && str[0] <= 'd') {
			return true;
		}

		return false;
	}

	static int toReg(const std::string &str) {
		return str[0] - 'a';
	}

	int run(const std::vector<std::string> &code) {
		while (this->pc < code.size()) {
			std::vector<std::string> toks = utils::strTok(code[this->pc], ' ');

			if (toks[0] == "cpy") {
				int dstReg = toReg(toks[2]);

				if (isReg(toks[1])) {
					this->reg[dstReg] = this->reg[toReg(toks[1])];

				} else {
					this->reg[dstReg] = utils::toInt(toks[1]);
				}
				this->pc++;

			} else if (toks[0] == "inc") {
				this->reg[toReg(toks[1])]++;
				this->pc++;

			} else if (toks[0] == "dec") {
				this->reg[toReg(toks[1])]--;
				this->pc++;

			} else if (toks[0] == "jnz") {
				int cVal;

				if (isReg(toks[1])) {
					cVal = this->reg[toReg(toks[1])];
				} else {
					cVal = utils::toInt(toks[1]);
				}

				if (cVal != 0) {
					this->pc += utils::toInt(toks[2]);
				} else {
					this->pc++;
				}

			} else {
				ERR(("Not supported opcode '%s'!", toks[0].c_str()));
				abort();
			}
		}

		return this->reg[0];
	}
};


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
