/*
 * computer.h
 *
 *  Created on: 19.11.2020
 *      Author: jarko
 */

#ifndef COMPUTER_H_
#define COMPUTER_H_

#include "common/types.h"


class Computer {
	public:
		struct BreakpointException {
		};

		void reset();

		Computer();
		virtual ~Computer() {}

		void setBreakpoint(int pc);

		int pc() const {
			return this->_pc;
		}

		int acc() const {
			return this->_acc;
		}

		void tick(const std::vector<std::string> &_code);
		void run(const std::vector<std::string> &code);

	private:
		long long int _acc;
		int _pc;
		int _breakpoint;
};


#endif /* 2016_DAY12_COMPUTER_H_ */
