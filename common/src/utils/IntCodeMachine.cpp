#include <stdexcept>

#include <utils/IntCodeMachine.h>

#define DEBUG_LEVEL 5
#include "common/debug.h"


IntCodeMachine::IntCodeMachine(const std::vector<int64_t> &program) : memory(program), program(program) {
	this->pc           = 0;
	this->eop          = false;
	this->relativeBase = 0;

	this->memory.resize(64 * 1024 * 1024);
}


IntCodeMachine::~IntCodeMachine() {

}


bool IntCodeMachine::onOut(int64_t value) {
	WRN(("Default implementation!"));

	return false;
}


bool IntCodeMachine::onIn(int64_t &value) {
	WRN(("Default implementation!"));

	return false;
}


bool IntCodeMachine::handleOpcode(std::vector<int64_t> &mem, int code, int64_t arg1, int64_t arg2, int64_t arg3) {
	DBG(("Code: %d, arg1: %" PRId64 ", arg2: %" PRId64 ", arg3: %" PRId64 " ", code, arg1, arg2, arg3));

	switch (code) {
		case 1:
		case 2:
			{
				int64_t noun = mem[arg1];
				int64_t verb = mem[arg2];
				int64_t result;

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
				int64_t inVal;

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

		case 9:
			{
				this->relativeBase += mem[arg1];

				this->pc += 2;
			}
			break;

		default:
			ERR(("Not supported code: %d", code));

			throw std::runtime_error("Not supported opcode!");
	}

	return true;
}



bool IntCodeMachine::run() {
	int64_t num;

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
			case 9:
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
						int64_t opcode = num % 100;
						int64_t arg1 = this->pc + 1;
						int64_t arg2 = this->pc + 2;
						int64_t arg3 = this->pc + 3;

						switch ((num / 100) % 10) {
							case 0: arg1 = memory[arg1];                      break;
							case 1:                                           break;
							case 2: arg1 = memory[arg1] + this->relativeBase; break;
						}

						switch ((num / 1000) % 10) {
							case 0: arg2 = memory[arg2];                      break;
							case 1:                                           break;
							case 2: arg2 = memory[arg2] + this->relativeBase; break;
						}

						switch ((num / 10000) % 10) {
							case 0: arg3 = memory[arg3];                      break;
							case 1:                                           break;
							case 2: arg3 = memory[arg3] + this->relativeBase; break;
						}

						if (! this->handleOpcode(memory, opcode, arg1, arg2, arg3)) {
							return false;
						}

					} else {
						ERR(("Not supported code: %" PRId64, num));

						throw std::runtime_error("Not supported opcode!");
					}
				}
				break;
		}
	} while (num != 99);

	return true;
}


void IntCodeMachine::reset() {
	this->pc           = 0;
	this->eop          = false;
	this->relativeBase = 0;

	std::copy(this->program.begin(), this->program.end(), this->memory.begin());
}


bool IntCodeMachine::finished() const {
	return this->eop;
}


std::vector<int64_t> &IntCodeMachine::getMemory() {
	return this->memory;
}
