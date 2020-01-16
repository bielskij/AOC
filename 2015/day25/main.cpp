#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int getIndex(int row, int column) {
	int ret = 1;

	for (int i = 1; i < row; i++) {
		ret += i;
	}

	for (int i = 1; i < column; i++) {
		ret += (row + i);
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto line = File::readAllLines(argv[1])[0];

	int row;
	int col;

	if (sscanf(line.c_str(), "To continue, please consult the code grid in the manual.  Enter the code at row %d, column %d.", &row, &col) != 2) {
		abort();
	}

	int pos = getIndex(row, col);
	uint64_t val = 20151125;

	for (int i = 1; i < pos; i++) {
		val = (val * 252533) % 33554393;
	}

	PRINTF(("PART_A: %d", (int) val));
}
