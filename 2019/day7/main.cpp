#include <iostream>
#include <cstring>
#include <algorithm>
#include <queue>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class Amplifier : public IntCodeMachine {
	public:
		Amplifier(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			this->phase     = 0;
			this->sendPhase = false;
			this->next      = NULL;
		}

		void reset() {
			this->sendPhase = true;

			this->outputs.clear();

			while (! this->inputs.empty()) {
				this->inputs.pop();
			}

			this->IntCodeMachine::reset();
		}

	protected:
		bool onOut(int64_t value) {
			LOG(("OUT> %" PRId64, value));

			this->outputs.push_back(value);

			if (this->next) {
				this->next->addInput(value);
			}

			return true;
		}

		bool onIn(int64_t &value) {
			if (this->sendPhase) {
				value = this->phase;

				this->sendPhase = false;
			} else {
				if (this->inputs.empty()) {
					return false;

				} else {
					value = this->inputs.front();

					this->inputs.pop();
				}
			}

			LOG(("[%p] IN< %" PRId64, this, value));

			return true;
		}

	public:
		void addInput(int input) {
			this->inputs.push(input);
		}

		void setPhase(int phase) {
			this->phase = phase;
		}

		const std::vector<int64_t> &getOutputs() const {
			return this->outputs;
		}

		void setNext(Amplifier *amp) {
			this->next = amp;
		}

	private:
		int phase;
		bool sendPhase;

		std::queue<int64_t> inputs;
		std::vector<int64_t> outputs;

		Amplifier *next;
};


static void _genPermutation(std::vector<std::vector<int>> &out, int a[], int size, int n) {
	if (size == 1) {
		std::vector<int> combination;

		combination.assign(a, a + n);
	
		out.push_back(combination);
		return;
	}

	for (int i = 0; i < size; i++) {
		_genPermutation(out, a, size - 1, n);

		if (size % 2 == 1) {
			std::swap(a[0], a[size - 1]);
		} else {
			std::swap(a[i], a[size - 1]);
		}
	}
}


int main(int argc, char *argv[]) {
	std::vector<int64_t> codes = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		int phases[] = { 0, 1, 2, 3, 4 };

		std::vector<std::vector<int>> samples;
		
		_genPermutation(samples, phases, 5, 5);

		Amplifier amplifier(codes);

		std::vector<int> outputs;
		for (auto sample = samples.begin(); sample != samples.end(); sample++) {
			int input = 0;

			for (int i = 0; i < 5; i++) {
				amplifier.reset();
				amplifier.addInput(input);
				amplifier.setPhase(sample->at(i));
				amplifier.run();

				input = amplifier.getOutputs()[0];
			}

			outputs.push_back(input);
		}

		PRINTF(("PART A: %d", *std::max_element(outputs.begin(), outputs.end())));
	}

	{
		int phases[] = { 5, 6, 7, 8, 9 };

		std::vector<std::vector<int>> samples;

#if 1
		_genPermutation(samples, phases, 5, 5);
#else
#if 1
		codes = {3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,
				27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5 };
		samples.push_back({9,8,7,6,5});
		// result 139629729
#else
		codes = {3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,
				-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,
				53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10 };

		samples.push_back({9,7,8,5,6});
		// result 18216
#endif
#endif

		std::vector<Amplifier> amplifiers;

		amplifiers.push_back(Amplifier(codes));
		amplifiers.push_back(Amplifier(codes));
		amplifiers.push_back(Amplifier(codes));
		amplifiers.push_back(Amplifier(codes));
		amplifiers.push_back(Amplifier(codes));

		for (int i = 0; i < amplifiers.size(); i++) {
			if (i == amplifiers.size() - 1) {
				amplifiers[i].setNext(&amplifiers[0]);
			} else {
				amplifiers[i].setNext(&amplifiers[i + 1]);
			}
		}

		int max = 0;
		for (auto sample = samples.begin(); sample != samples.end(); sample++) {
			for (int i = 0; i < amplifiers.size(); i++) {
				amplifiers[i].reset();
				amplifiers[i].setPhase(sample->at(i));
			}

			amplifiers[0].addInput(0);

			bool allFinished;
			do {
				allFinished = true;

				for (auto amp = amplifiers.begin(); amp != amplifiers.end(); amp++) {
					amp->run();
				}

				for (auto amp = amplifiers.begin(); amp != amplifiers.end(); amp++) {
					if (! amp->finished()) {
						allFinished = false;
					}
				}
			} while (! allFinished);

			const std::vector<int64_t> &outputs = amplifiers.rbegin()->getOutputs();
			if (*outputs.rbegin() > max) {
				max = *outputs.rbegin();
			}
		}

		PRINTF(("PART B: %d", max));
	}
}
