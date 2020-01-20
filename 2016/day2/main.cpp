#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


static char valuesA[] = {
	'1', '2', '3',
	'4', '5', '6',
	'7', '8', '9'
};


static char valuesB[] = {
	 0,    0, '1',   0,  0,
	 0,  '2', '3', '4',  0,
	'5', '6', '7', '8', '9',
	 0,  'A', 'B', 'C',  0,
	 0,    0, 'D',   0,  0,
};


static char getValueA(const Point<int> &pos) {
	if (pos.x() < 0 || pos.x() >= 3 || pos.y() < 0 || pos.y() >= 3) {
		return 0;
	}

	return valuesA[pos.y() * 3 + pos.x()];
}


static char getValueB(const Point<int> &pos) {
	if (pos.x() < 0 || pos.x() >= 5 || pos.y() < 0 || pos.y() >= 5) {
		return 0;
	}

	return valuesB[pos.y() * 5 + pos.x()];
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	{
		Point<int> pos(1, 1);

		std::string code;

		for (auto &l : data) {
			for (char c : l) {
				Point<int> tmpPos = pos;

				switch (c) {
					case 'U': tmpPos.decY(); break;
					case 'D': tmpPos.incY(); break;
					case 'R': tmpPos.incX(); break;
					case 'L': tmpPos.decX(); break;
				}

				if (getValueA(tmpPos) != 0) {
					pos = tmpPos;
				}
			}

			code.push_back(getValueA(pos));
		}

		PRINTF(("PART_A: %s", code.c_str()));
	}

	{
		Point<int> pos(0, 2);

		std::string code;

		for (auto &l : data) {
			for (char c : l) {
				Point<int> tmpPos = pos;

				switch (c) {
					case 'U': tmpPos.decY(); break;
					case 'D': tmpPos.incY(); break;
					case 'R': tmpPos.incX(); break;
					case 'L': tmpPos.decX(); break;
				}

				if (getValueB(tmpPos) != 0) {
					pos = tmpPos;
				}
			}

			code.push_back(getValueB(pos));
		}

		PRINTF(("PART_B: %s", code.c_str()));
	}
}
