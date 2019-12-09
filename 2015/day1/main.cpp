#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::string data = File::readAllLines(argv[1])[0];

	{
		int floor = 0;

		for (auto c = data.begin(); c != data.end(); c++) {
			switch (*c) {
				case '(': floor++; break;
				case ')': floor--; break;

				default:
					throw std::runtime_error("Not supported code!");
			}
		}

		PRINTF(("PART A: %d", floor));
	}

	{
		int floor = 0;

		for (int i = 0; i < data.length(); i++) {
			switch (data[i]) {
				case '(': floor++; break;
				case ')': floor--; break;

				default:
					throw std::runtime_error("Not supported code!");
			}

			if (floor == -1) {
				PRINTF(("PART B: %d", i + 1));

				break;
			}
		}
	}
}
