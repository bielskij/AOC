/*
 * computer.cpp
 *
 *  Created on: 19.11.2020
 *      Author: jarko
 */
#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#include "computer.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


void Computer::reset() {
	this->pc = 0;

	for (auto &r : reg) {
		r = 0;
	}
}


Computer::Computer() {
	this->reset();
}


bool Computer::isReg(const std::string &str) {
	if (str.length() != 1) {
		return false;
	}

	if (str[0] >= 'a' && str[0] <= 'd') {
		return true;
	}

	return false;
}

int Computer::toReg(const std::string &str) {
	return str[0] - 'a';
}

int Computer::run(const std::vector<std::string> &_code) {
	std::vector<std::string> code = _code;

	while (this->pc < code.size()) {
		std::vector<std::string> toks = utils::strTok(code[this->pc], ' ');

//		DBG(("PC: %u, instruction: %s, a: %lld, b: %lld, c: %lld, d: %lld",
//			this->pc, code[this->pc].c_str(), this->reg[0], this->reg[1], this->reg[2], this->reg[3]
//		));

		if (toks[0] == "cpy") {
			if (! isReg(toks[2])) {
				ERR(("Argument '%s' is not a register! - skipping", toks[2].c_str()));

			} else {
				int dstReg = toReg(toks[2]);

				if (isReg(toks[1])) {
					this->reg[dstReg] = this->reg[toReg(toks[1])];

				} else {
					this->reg[dstReg] = utils::toInt(toks[1]);
				}
			}
			this->pc++;

		} else if (toks[0] == "inc") {
			if (! isReg(toks[1])) {
				ERR(("Argument '%s' is not a register!", toks[1].c_str()));

			} else {
				this->reg[toReg(toks[1])]++;
			}
			this->pc++;

		} else if (toks[0] == "dec") {
			if (! isReg(toks[1])) {
				ERR(("Argument '%s' is not a register!", toks[1].c_str()));

			} else {
				this->reg[toReg(toks[1])]--;
			}
			this->pc++;

		} else if (toks[0] == "jnz") {
			int cVal;

			if (isReg(toks[1])) {
				cVal = this->reg[toReg(toks[1])];
			} else {
				cVal = utils::toInt(toks[1]);
			}

			if (cVal != 0) {
				if (isReg(toks[2])) {
					this->pc += this->reg[toReg(toks[2])];
				} else {
					this->pc += utils::toInt(toks[2]);
				}

			} else {
				this->pc++;
			}

		} else if (toks[0] == "tgl") {
			int                      modInstructionPc = this->pc + (isReg(toks[1]) ? this->reg[toReg(toks[1])] : utils::toInt(toks[1]));
			std::vector<std::string> modInstructionToks;

			if (modInstructionPc >= code.size()) {
				ERR(("tgl instruction tries to modify instruction outside code range!"));

			} else {
				modInstructionToks = utils::strTok(code[modInstructionPc], ' ');

				switch (modInstructionToks.size()) {
					case 2:
						if (modInstructionToks[0] == "inc") {
							code[modInstructionPc] = "dec " + modInstructionToks[1];
						} else {
							code[modInstructionPc] = "inc " + modInstructionToks[1];
						}
						break;

					case 3:
						if (modInstructionToks[0] == "jnz") {
							code[modInstructionPc] = "cpy " + modInstructionToks[1] + " " + modInstructionToks[2];
						} else {
							code[modInstructionPc] = "jnz " + modInstructionToks[1] + " " + modInstructionToks[2];
						}
						break;

					default:
						abort();
				}
			}

			this->pc++;

		} else if (toks[0] == "mul") {
			int factor1 = isReg(toks[1]) ? this->reg[toReg(toks[1])] : utils::toInt(toks[1]);
			int factor2 = isReg(toks[2]) ? this->reg[toReg(toks[2])] : utils::toInt(toks[2]);

			this->reg[toReg(toks[3])] = factor1 * factor2;

			this->pc++;

		} else if (toks[0] == "nop") {
			this->pc++;

		} else {
			ERR(("Not supported opcode '%s'!", toks[0].c_str()));
			abort();
		}
	}

	return this->reg[0];
}
