#include <stdexcept>

#include <utils/IntCodeMachine.h>
#include <utils/utils.h>

#define DEBUG_LEVEL 5
#include "common/debug.h"


IntCodeMachine::IntCodeMachine(const std::vector<int64_t> &program) : memory(program), program(program) {
	this->pc           = 0;
	this->eop          = false;
	this->relativeBase = 0;

	this->memory.resize(64 * 1024);
}


IntCodeMachine::~IntCodeMachine() {

}


bool IntCodeMachine::onOut(int64_t value) {
	WRN(("%s(): Default implementation!", __func__));

	return false;
}


bool IntCodeMachine::onIn(int64_t &value) {
	WRN(("%s(): Default implementation!", __func__));

	return false;
}


void IntCodeMachine::onMemoryWrite(int64_t address, int64_t currentValue, int64_t newValue) {
	WRN(("%s(): Default implementation!", __func__));
}


void IntCodeMachine::onMemoryRead(int64_t address, int64_t currentValue) {
	WRN(("%s(): Default implementation!", __func__));
}

#define _I(x) ((int)x)
#define _IP PRId64


std::string IntCodeMachine::_addrToAsm(const std::vector<int64_t> &program, int argMode, int64_t arg, bool readPositionParameters) {
	std::string ret;

	if (argMode == ADDRESS_IMMEDIATE) {
		ret = utils::toString(arg);
	} else {
		if (readPositionParameters) {
			ret = "mem[" + utils::toString(program[arg]);
		} else {
			ret = "mem[mem[" + utils::toString(arg);
		}

		if (argMode == ADDRESS_RELATIVE) {
			ret += " + _rel";
		}

		if (! readPositionParameters) {
			ret += "]";
		}
		ret += "]";
	}

	return ret;
}


std::string IntCodeMachine::_opcodeToAsm(const std::vector<int64_t> &program, int pc, int code, int arg1Mode, int64_t arg1, int arg2Mode, int64_t arg2, int arg3Mode, int64_t arg3, int &codeSize, bool rpp) {
	std::string _asm = utils::toString(pc);

	switch (code) {
			case 1:
			case 2:
				{
					if (code == 1) {
						_asm += " add ";
					} else {
						_asm += " mul ";
					}

					_asm += _addrToAsm(program, arg3Mode, arg3, rpp) + " := " + _addrToAsm(program, arg1Mode, arg1, rpp) + ", " + _addrToAsm(program, arg2Mode, arg2, rpp);

					codeSize = 4;
				}
				break;

			case 3:
				{
					_asm += " in " + _addrToAsm(program, arg1Mode, arg1, rpp);
					codeSize = 2;
				}
				break;

			case 4:
				{
					_asm += " out " + _addrToAsm(program, arg1Mode, arg1, rpp);
					codeSize = 2;
				}
				break;

			case 5:
				{
					_asm += " jmp_gtz " + _addrToAsm(program, arg1Mode, arg1, rpp) + ", " + _addrToAsm(program, arg2Mode, arg2, rpp);
					codeSize = 3;
				}
				break;

			case 6:
				{
					_asm += " jmp_eqz " + _addrToAsm(program, arg1Mode, arg1, rpp) + ", " + _addrToAsm(program, arg2Mode, arg2, rpp);
					codeSize = 3;
				}
				break;

			case 7:
				{
					_asm += " cmp_lt " + _addrToAsm(program, arg3Mode, arg3, rpp) + " := " + _addrToAsm(program, arg1Mode, arg1, rpp) + ", " + _addrToAsm(program, arg2Mode, arg2, rpp);
					codeSize = 4;
				}
				break;

			case 8:
				{
					_asm += " cmp_eq " + _addrToAsm(program, arg3Mode, arg3, rpp) + " := " + _addrToAsm(program, arg1Mode, arg1, rpp) + ", " + _addrToAsm(program, arg2Mode, arg2, rpp);
					codeSize = 4;
				}
				break;

			case 9:
				{
					_asm += " rel " + _addrToAsm(program, arg1Mode, arg1, rpp);
					codeSize = 2;
				}
				break;

			case 99:
				{
					_asm += " halt";
					codeSize = 1;
				}
				break;
	}

	return _asm;
}


void IntCodeMachine::_memWrite(int64_t address, int64_t value) {

	if (! this->watchedAddresses.empty()) {
		int64_t currentValue = this->memory[address];

		if (this->watchedAddresses.find(address) != this->watchedAddresses.end()) {
			this->onMemoryWrite(address, currentValue, value);
		}
	}

	this->memory[address] = value;
}


int64_t IntCodeMachine::_memRead(int64_t address) {
	int64_t val = this->memory[address];

	if (! this->watchedAddresses.empty()) {
		if (this->watchedAddresses.find(address) != this->watchedAddresses.end()) {
			this->onMemoryRead(address, val);
		}
	}

	return val;
}


bool IntCodeMachine::handleOpcode(int code, int64_t arg1, int64_t arg2, int64_t arg3) {
	switch (code) {
		case 1:
		case 2:
			{
				int64_t noun = _memRead(arg1);
				int64_t verb = _memRead(arg2);
				int64_t result;

				if (code == 1) {
					result = noun + verb;
				} else {
					result = noun * verb;
				}

				DBG(("[%" _IP "] %s m[%" _IP "] (%" _IP ") = m[%" _IP "] (%" _IP ") + m[%" _IP "] (%" _IP ")", this->pc, code == 1 ? "ADD" : "MUL", arg3, result, arg1, noun, arg2, verb));

				_memWrite(arg3, result);

				this->pc += 4;
			}
			break;

		case 3:
			{
				int64_t inVal;

				if (! this->onIn(inVal)) {
					return false;

				} else {
					_memWrite(arg1, inVal);

					DBG(("[%" _IP "] IN m[%" _IP "] = %" _IP, this->pc, arg1, inVal));

					pc += 2;
				}
			}
			break;

		case 4:
			{
				int64_t val = _memRead(arg1);
				if (! this->onOut(val)) {
					return false;

				} else {
					DBG(("[%" _IP "] OUT m[%" _IP "] = %" _IP, this->pc, arg1, val));

					pc += 2;
				}
			}
			break;

		case 5:
			{
				int64_t arg1Val = _memRead(arg1);
				int64_t arg2Val = _memRead(arg2);

				DBG(("[%" _IP "] m[%" _IP "] (%" _IP ") JMP_GT 0  PC <- m[%" _IP "] (%" _IP ")", this->pc, arg1, arg1Val, arg2, arg2Val));

				if (arg1Val > 0) {
					pc = arg2Val;

				} else {
					pc += 3;
				}
			}
			break;

		case 6:
			{
				int64_t arg1Val = _memRead(arg1);
				int64_t arg2Val = _memRead(arg2);

				DBG(("[%" _IP "] m[%" _IP "] (%" _IP ") JMP_EQ 0  PC <- m[%" _IP "] (%" _IP ")", this->pc, arg1, arg1Val, arg2, arg2Val));

				if (arg1Val == 0) {
					pc = arg2Val;

				} else {
					pc += 3;
				}
			}
			break;

		case 7:
			{
				int64_t arg1Val = _memRead(arg1);
				int64_t arg2Val = _memRead(arg2);
				int64_t outVal  = (arg1Val < arg2Val) ? 1 : 0;

				DBG(("[%" _IP "] m[%" _IP "] (%" _IP ") LT m[%" _IP "] (%" _IP ")   m[%" _IP "] = %" _IP,
					this->pc, arg1, arg1Val, arg2, arg2Val, arg3, outVal
				));

				_memWrite(arg3, outVal);

				pc += 4;
			}
			break;

		case 8:
			{
				int64_t arg1Val = _memRead(arg1);
				int64_t arg2Val = _memRead(arg2);
				int64_t outVal  = (arg1Val == arg2Val) ? 1 : 0;

				DBG(("[%" _IP "] m[%" _IP "] (%" _IP ") EQ m[%" _IP "] (%" _IP ")   m[%" _IP "] = %" _IP,
					this->pc, arg1, arg1Val, arg2, arg2Val, arg3, outVal
				));

				_memWrite(arg3, outVal);

				pc += 4;
			}
			break;

		case 9:
			{
				this->relativeBase += _memRead(arg1);

				this->pc += 2;
			}
			break;

		default:
			ERR(("Not supported code: %d", code));

			throw std::runtime_error("Not supported opcode!");
	}

	return true;
}


bool IntCodeMachine::step() {
	if (this->finished()) {
		return false;
	}

	int64_t num = this->memory.at(this->pc);
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
			if (! this->handleOpcode(num, memory[this->pc + 1], memory[this->pc + 2], memory[this->pc + 3])) {
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

					if (! this->handleOpcode(opcode, arg1, arg2, arg3)) {
						return false;
					}

				} else {
					ERR(("Not supported code: %" PRId64 ", at: %" PRId64, num, this->pc));

					throw std::runtime_error("Not supported opcode!");
				}
			}
			break;
	}

	return true;
}


bool IntCodeMachine::run() {
	int64_t num;

	if (this->finished()) {
		return false;
	}

	do {
		if (! this->step()) {
			return false;
		}
	} while (! this->finished());

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


std::string IntCodeMachine::getAsm(bool readPositionAddresses) {
	std::string ret;

	for (int i = 0; i < this->program.size(); i++) {
		int64_t num = this->program.at(i);
		int codeSize;

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
			case 99:
				ret += _opcodeToAsm(this->program, i, num, ADDRESS_POSITION, i + 1, ADDRESS_POSITION, i + 2, ADDRESS_POSITION, i + 3, codeSize, readPositionAddresses);
				break;

			default:
				{
					if (num > 100) {
						int64_t opcode = num % 100;
						int64_t arg1 = i + 1;
						int64_t arg2 = i + 2;
						int64_t arg3 = i + 3;

						ret += _opcodeToAsm(this->program, i, opcode, (num / 100) % 10, arg1, (num / 1000) % 10, arg2, (num / 10000) % 10, arg3, codeSize, readPositionAddresses);

					} else {
						codeSize = 0;
					}
				}
				break;
		}

		if (codeSize > 0) {
			ret += "\n";

			i += (codeSize - 1);
		}
	}

	return ret;
}


void IntCodeMachine::setPc(int64_t pc) {
	this->pc = pc;
}


void IntCodeMachine::addMemoryWatch(int64_t address) {
	this->watchedAddresses.insert(address);
}
