#include <stack>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Alu {
	enum Reg {
		REG_W,
		REG_X,
		REG_Y,
		REG_Z,

		REG_COUNT
	};

	int64_t regs[REG_COUNT];
	int pc;
	std::vector<std::string> code;
	std::string              args;
	int                      argsOffset;


	Alu(const std::string &args) : args(args) {
		this->reset();
	}

	void load(const std::vector<std::string> &code) {
		this->code = code;
	}

	void reset() {
		this->regs[REG_W] = 0;
		this->regs[REG_X] = 0;
		this->regs[REG_Y] = 0;
		this->regs[REG_Z] = 0;

		this->pc         = 0;
		this->argsOffset = 0;
	}

	void run() {
		while (this->pc < this->code.size()) {
			const std::string &inst = this->code[this->pc];

			char operandRight[10] = { 0 };
			char operandLeft;

			if (sscanf(inst.c_str(), "inp %c", &operandLeft) == 1) {
				this->regs[operandLeft - 'w'] = this->args[this->argsOffset++] - '0';

			} else if (sscanf(inst.c_str(), "add %c %s", &operandLeft, operandRight) == 2) {
				this->regs[operandLeft - 'w'] += ::isalpha(operandRight[0]) ? this->regs[operandRight[0] - 'w'] : std::stoi(operandRight);

			} else if (sscanf(inst.c_str(), "mod %c %s", &operandLeft, operandRight) == 2) {
				this->regs[operandLeft - 'w'] %= ::isalpha(operandRight[0]) ? this->regs[operandRight[0] - 'w'] : std::stoi(operandRight);

			} else if (sscanf(inst.c_str(), "div %c %s", &operandLeft, operandRight) == 2) {
				this->regs[operandLeft - 'w'] /= ::isalpha(operandRight[0]) ? this->regs[operandRight[0] - 'w'] : std::stoi(operandRight);

			} else if (sscanf(inst.c_str(), "mul %c %s", &operandLeft, operandRight) == 2) {
				this->regs[operandLeft - 'w'] *= ::isalpha(operandRight[0]) ? this->regs[operandRight[0] - 'w'] : std::stoi(operandRight);

			} else if (sscanf(inst.c_str(), "eql %c %s", &operandLeft, operandRight) == 2) {
				int64_t &reg = this->regs[operandLeft - 'w'];

				if (reg == (::isalpha(operandRight[0]) ? this->regs[operandRight[0] - 'w'] : std::stoi(operandRight))) {
					reg = 1;

				} else {
					reg = 0;
				}

			} else {
				PRINTF(("Unknown operation! (%s)", inst.c_str()));
				abort();
			}

			this->pc++;
		}
	}
};


std::string _findNumber(const std::vector<std::string> &code, bool min) {
	int blockLength = code.size() / 14;

	std::string number(14, min ? 1 : 9);
	std::stack<std::pair<int, int>> stack;

	for (int digitNo = 0 ; digitNo < 14; digitNo++) {
		int zDivisor = std::stoi(utils::strTok(code[digitNo * blockLength + 4], ' ')[2]); // div z []

		DBG(("Divisor: %d", zDivisor));

		switch (zDivisor) {
			case 1:
				{
					int remainderModifier = std::stoi(utils::strTok(code[digitNo * blockLength + 15], ' ')[2]); // add y []

					DBG(("Remainder modifier: %d", remainderModifier));

					stack.push(std::pair<int, int>(digitNo, remainderModifier));
				}
				break;

			case 26:
				{
					const auto digitModifier = stack.top();

					stack.pop();

					int compareModifier = std::stoi(utils::strTok(code[digitNo * blockLength + 5], ' ')[2]); // add x []

					number[digitNo] = number[digitModifier.first] + digitModifier.second + compareModifier;

					if (number[digitNo] > 9) {
						number[digitModifier.first] -= (number[digitNo] - 9);
						number[digitNo]              = 9;

					} else if (number[digitNo] < 1) {
						number[digitModifier.first] += (1 - number[digitNo]);
						number[digitNo]              = 1;
					}
				}
				break;

			default:
				abort();
		}
	}

	for (int i = 0; i < 14; i++) {
		number[i] += '0';
	}

	return number;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		Alu alu(_findNumber(lines, false));

		alu.load(lines);

		alu.run();

		PRINTF(("PART_A: %s (Z: %ld)", alu.args.c_str(), alu.regs[Alu::REG_Z]));
	}

	{
		Alu alu(_findNumber(lines, true));

		alu.load(lines);

		alu.run();

		PRINTF(("PART_B: %s (Z: %ld)", alu.args.c_str(), alu.regs[Alu::REG_Z]));
	}

	return 0;
}
