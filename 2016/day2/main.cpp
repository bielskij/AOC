#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	Point<int> pos(1, 1);

	std::string code;

	for (auto &l : data) {
		for (char c : l) {
			switch (c) {
				case 'U':
					if (pos.y() > 0) {
						pos.decY();
					}
					break;

				case 'D':
					if (pos.y() + 1 < 3) {
						pos.incY();
					}
					break;

				case 'R':
					if (pos.x() + 1 < 3) {
						pos.incX();
					}
					break;

				case 'L':
					if (pos.x() > 0) {
						pos.decX();
					}
					break;
			}
		}

		code.push_back('0' + pos.y() * 3 + pos.x() + 1);
	}

	PRINTF(("PART_A: %s", code.c_str()));
}
