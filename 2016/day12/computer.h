/*
 * computer.h
 *
 *  Created on: 19.11.2020
 *      Author: jarko
 */

#ifndef COMPUTER_H_
#define COMPUTER_H_

#include "common/types.h"


struct Computer {
	struct BreakpointException {
	};

	long long int reg[4];
	int pc;
	int breakpoint;

	void reset();

	Computer();
	virtual ~Computer() {}

	static bool isReg(const std::string &str);

	static int toReg(const std::string &str);

	void setBreakpoint(int pc);

	int run(const std::vector<std::string> &code);

	virtual void onOut(int val);
};


#endif /* 2016_DAY12_COMPUTER_H_ */
