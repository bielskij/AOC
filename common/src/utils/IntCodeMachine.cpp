#include <stdexcept>

#include <utils/IntCodeMachine.h>

#include "common/debug.h"


IntCodeMachine::IntCodeMachine(const std::vector<int> &program) : memory(program), program(program) {
	this->pc  = 0;
	this->eop = false;
}


IntCodeMachine::~IntCodeMachine() {

}


bool IntCodeMachine::onOut(int value) {
	WRN(("Default implementation!"));

	return false;
}


bool IntCodeMachine::onIn(int &value) {
	WRN(("Default implementation!"));

	return false;
}


bool IntCodeMachine::handleOpcode(std::vector<int> &mem, int code, int arg1, int arg2, int arg3) {
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
				int inVal;

				if (! this->onIn(inVal)) {
					return false;

				} else {
					mem[arg1] = inVal;

					pc += 2;
				}				
			}
			break;

		case 4:
			{
				if (! this->onOut(mem[arg1])) {
					return false;

				} else {
					pc += 2;
				}				
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

	return true;
}



bool IntCodeMachine::run() {
	int num;

	if (this->finished()) {
		return false;
	}

	do {
		num = this->memory.at(this->pc);

		switch (num) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				if (! this->handleOpcode(this->memory, num, memory[this->pc + 1], memory[this->pc + 2], memory[this->pc + 3])) {
					return false;
				}
				break;

			case 99:
				this->eop = true;
				break;

			default:
				{
					if (num > 100) {
						int opcode = num % 100;
						int arg1 = this->pc + 1;
						int arg2 = this->pc + 2;
						int arg3 = this->pc + 3;

						if ((num % 1000) < 100) {
							arg1 = memory[arg1];
						}

						if ((num % 10000) < 1000) {
							arg2 = memory[arg2];
						}

						if ((num % 100000) < 10000) {
							arg3 = memory[arg3];
						}

						if (! this->handleOpcode(memory, opcode, arg1, arg2, arg3)) {
							return false;
						}

					} else {
						ERR(("Not supported code: %d", num));

						throw std::runtime_error("Not supported opcode!");
					}
				}
				break;
		}
	} while (num != 99);

	return true;
}


void IntCodeMachine::reset() {
	this->memory = this->program;
	this->pc     = 0;
	this->eop    = false;
}


bool IntCodeMachine::finished() const {
	return this->eop;
}


std::vector<int> &IntCodeMachine::getMemory() {
	return this->memory;
}
