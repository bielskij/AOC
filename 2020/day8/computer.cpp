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
	this->_pc  = 0;
	this->_acc = 0;
}


Computer::Computer() {
	this->_breakpoint = -1;

	this->reset();
}


void Computer::tick(const std::vector<std::string> &code) {
	std::vector<std::string> toks = utils::strTok(code[this->pc()], ' ');

	if (toks[0] == "acc") {
		this->_acc += utils::toInt(toks[1]);
		this->_pc++;

	} else if (toks[0] == "nop") {
		this->_pc++;

	} else if (toks[0] == "jmp") {
		this->_pc += utils::toInt(toks[1]);

	} else {
		ERR(("Not supported opcode '%s'!", toks[0].c_str()));
		abort();
	}

	if (this->_breakpoint >= 0) {
		if (this->_pc == this->_breakpoint) {
			throw BreakpointException();
		}
	}
}


void Computer::run(const std::vector<std::string> &code) {
	while (this->pc() < code.size()) {
		this->tick(code);
	}
}


void Computer::setBreakpoint(int pc) {
	this->_breakpoint = pc;
}
