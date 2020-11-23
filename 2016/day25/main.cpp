#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#include "computer.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct ClockComputer : public Computer {
	std::vector<int> pulses;
	int              startValue;

	void onOut(int val) {
		pulses.push_back(val);
	}

	bool isCorrectClock() {
		if (*pulses.begin() == *pulses.rbegin()) {
			return false;
		}

		int prev = pulses[0] ^ 1;

		for (auto &p : pulses) {
			if (prev ^ p == 0) {
				return false;
			}

			prev = p;
		}

		return true;
	}

	void resetClock() {
		this->pulses.clear();
	}

	int solve(const std::vector<std::string> &code) {
		this->startValue = 0;

		this->setBreakpoint(29);

		do {
			this->pulses.clear();

			this->reset();

			this->reg[0] = this->startValue++;

			try {
				this->run(code);
			} catch (const BreakpointException &) {}

		} while (! this->isCorrectClock());

		return this->startValue - 1;
	}
};


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	// The code outputs remainder from division by 2 of value of the expression 'a + b (231) * c (11)'
	{
		ClockComputer c;

		PRINTF(("RESULT: %d", c.solve(data)));
	}
}
