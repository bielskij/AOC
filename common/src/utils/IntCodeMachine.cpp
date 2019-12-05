#include <stdexcept>

#include <utils/IntCodeMachine.h>

#include "common/debug.h"


IntCodeMachine::IntCodeMachine() {
	this->pc = 0;
}


IntCodeMachine::~IntCodeMachine() {

}


void IntCodeMachine::onOut(int value) {
	WRN(("Default implementation!"));
}


int IntCodeMachine::onIn() {
	WRN(("Default implementation!"));
	return 0;
}


void IntCodeMachine::handleOpcode(std::vector<int> &mem, int code, int arg1, int arg2, int arg3) {
	switch (code) {
		case 1:
		case 2:
			{
				int noun = mem[arg1];
				int verb = mem[arg2];
				int result;

				if (code == 1) {
					result = noun + verb;
				} else {
					result = noun * verb;
				}

				mem[arg3] = result;

				this->pc += 4;
			}
			break;

		case 3:
			{
				mem[arg1] = this->onIn();

				pc += 2;
			}
			break;

		case 4:
			{
				this->onOut(mem[arg1]);

				pc += 2;
			}
			break;

		case 5:
			if (mem[arg1] > 0) {
				pc = mem[arg2];
			} else {
				pc += 3;
			}
			break;

		case 6:
			if (mem[arg1] == 0) {
				pc = mem[arg2];
			} else {
				pc += 3;
			}
			break;

		case 7:
			if (mem[arg1] < mem[arg2]) {
				mem[arg3] = 1;
			} else {
				mem[arg3] = 0;
			}
			pc += 4;
			break;

		case 8:
			if (mem[arg1] == mem[arg2]) {
				mem[arg3] = 1;
			} else {
				mem[arg3] = 0;
			}
			pc += 4;
			break;

		default:
			ERR(("Not supported code: %d", code));

			throw std::runtime_error("Not supported opcode!");
	}
}



void IntCodeMachine::run(std::vector<int> &program) {
	int num;

	this->pc = 0;

	do {
		num = program.at(this->pc);

		switch (num) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				this->handleOpcode(program, num, program[this->pc + 1], program[this->pc + 2], program[this->pc + 3]);
				break;

			case 99:
				LOG(("EOA"));
				break;

			default:
				{
					if (num > 100) {
						int opcode = num % 100;
						int arg1 = this->pc + 1;
						int arg2 = this->pc + 2;
						int arg3 = this->pc + 3;

						if ((num % 1000) < 100) {
							arg1 = program[arg1];
						}

						if ((num % 10000) < 1000) {
							arg2 = program[arg2];
						}

						if ((num % 100000) < 10000) {
							arg3 = program[arg3];
						}

						this->handleOpcode(program, opcode, arg1, arg2, arg3);

					} else {
						ERR(("Not supported code: %d", num));

						throw std::runtime_error("Not supported opcode!");
					}
				}
				break;
		}
	} while (num != 99);
}
