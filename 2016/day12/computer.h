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
	long long int reg[4];
	int pc;

	void reset();

	Computer();

	static bool isReg(const std::string &str);

	static int toReg(const std::string &str);

	int run(const std::vector<std::string> &code);
};


#endif /* 2016_DAY12_COMPUTER_H_ */
