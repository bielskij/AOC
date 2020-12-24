#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Ring {
	Ring(const std::string &str) {
		this->_cups.resize(str.length() + 1);

		this->_current = str[0] - '0';

		{
			int previous = this->_current;

			for (int i = 1; i < str.size(); i++) {
				this->_cups[previous] = str[i] - '0';

				previous = this->_cups[previous];
			}

			this->_cups[previous] = this->_current;
		}
	}

	void move() {
		int first = this->_cups[this->_current];
		int last  = this->_cups[this->_cups[first]];

		this->_cups[this->_current] = this->_cups[last];

		int destination = this->_current - 1 ? this->_current - 1 : this->_cups.size() - 1;

		bool removed = true;
		while (removed) {
			int idx = first;

			removed = false;

			for (int i = 0; i < 3; i++) {
				if (destination == idx) {
					removed = true;

					destination = destination - 1 ? destination - 1 : this->_cups.size() - 1;
				}

				idx = this->_cups[idx];
			}
		}

		this->_cups[last]        = this->_cups[destination];
		this->_cups[destination] = first;

		this->_current = this->_cups[this->_current];
	}

	void resize(int newSize) {
		int oldSize = this->_cups.size();
		int last    = this->_current;

		this->_cups.resize(newSize + 1);

		while (this->_cups[last] != this->_current) {
			last = this->_cups[last];
		}

		for (int i = oldSize; i <= newSize; i++) {
			this->_cups[last] = i;

			last = i;
		}

		this->_cups[last] = this->_current;
	}

	std::string partA() {
		std::string ret;

		{
			int next = this->_cups[1];

			do {
				ret.push_back(next + '0');

				next = this->_cups[next];
			} while (next != 1);
		}

		return ret;
	}

	long long int partB() {
		return (long long int) this->_cups[1] * (long long int) this->_cups[this->_cups[1]];
	}

	int _current;
	std::vector<int> _cups;
};


int main(int argc, char *argv[]) {
	std::string start = File::readAllLines(argv[1])[0];

	{
		Ring r(start);

		for (int turn = 0; turn < 100; turn++) {
			r.move();
		}

		PRINTF(("PART_A: %s", r.partA().c_str()));
	}

	{
		Ring r(start);

		r.resize(1000000);

		for (int turn = 0; turn < 10000000; turn++) {
			r.move();
		}

		PRINTF(("PART_B: %lld", r.partB()));
	}

	return 0;
}
