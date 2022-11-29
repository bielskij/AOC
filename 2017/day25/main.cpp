#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <string>
#include <cstring>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class Tape {
	public:
		void moveRight() {
			this->_pos++;
		}

		void moveLeft() {
			this->_pos--;
		}

		bool get() {
			return this->_tape[this->_pos];
		}

		void put(bool val) {
			this->_tape[this->_pos] = val;
		}

		void dump() {
			for (auto &p : _tape) {
				if (p.first == _pos) {
					printf("[%d] ", p.second);

				} else {
					printf("%d ", p.second);
				}
			}
			printf("\n");
		}

		int checksum() {
			int ret = 0;

			for (auto &p : _tape) {
				ret += (p.second ? 1 : 0);
			}

			return ret;
		}

	private:
		std::map<int, bool> _tape;
		int _pos;
};


struct Operations {
	bool writeValue;
	bool moveForward;
	char nextState;
};


class State {
	public:
		char process(Tape &tape) {
			Operations *ops = tape.get() ? &_onHi : &_onLo;

			tape.put(ops->writeValue);

			if (ops->moveForward) {
				tape.moveRight();
			} else {
				tape.moveLeft();
			}

			return ops->nextState;
		}

		Operations &getOps(bool hi) {
			if (hi) {
				return _onHi;

			} else {
				return _onLo;
			}
		}

	private:
		Operations _onHi;
		Operations _onLo;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		char    startState;
		int64_t steps;
		std::map<char, State> states;

		{
			char currentState;
			char onValue;
			char tmp;
			Operations ops;
			char direction[6];

			for (auto &line : lines) {
				auto str = utils::trim(line, " ");

				if (str.empty()) {
					continue;
				}

				if (sscanf(str.c_str(), "Begin in state %c.", &startState) == 1) {

				} else if (sscanf(str.c_str(), "Perform a diagnostic checksum after %"SCNd64" steps.", &steps) == 1) {

				} else if (sscanf(str.c_str(), "In state %c:", &currentState) == 1) {

				} else if (sscanf(str.c_str(), "If the current value is %d:", &onValue) == 1) {

				} else if (sscanf(str.c_str(), "- Write the value %c.", &tmp) == 1) {
					states[currentState].getOps(onValue).writeValue = tmp - '0';

				} else if (sscanf(str.c_str(), "- Move one slot to the %[^.]", &direction) == 1) {
					if (strcmp(direction, "right") == 0) {
						states[currentState].getOps(onValue).moveForward = true;

					} else if (strcmp(direction, "left") == 0) {
						states[currentState].getOps(onValue).moveForward = false;

					} else {
						ERR(("Invalid direction! '%s'", direction));
						abort();
					}

				} else if (sscanf(str.c_str(), "- Continue with state %c.", &tmp) == 1) {
					states[currentState].getOps(onValue).nextState = tmp;

				} else {
					ERR(("Not supported line! '%s'", str.c_str()));
				}
			}
		}

		{
			int64_t executedSteps = 0;

			Tape tape;

			char currentState = startState;

			while (executedSteps++ != steps) {
				auto &state = states[currentState];

				currentState = state.process(tape);

//				tape.dump();
			}

			PRINTF(("PART_A: %d", tape.checksum()));
		}
	}

	return 0;
}
