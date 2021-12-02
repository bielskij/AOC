#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		Point3d<int> pos;

		for (auto &r : lines) {
			int val;

			if (sscanf(r.c_str(), "forward %d", &val) == 1) {
				pos.setX(pos.getX() + val);

			} else if (sscanf(r.c_str(), "down %d", &val) == 1) {
				pos.setZ(pos.getZ() + val);

			} else if (sscanf(r.c_str(), "up %d", &val) == 1) {
				pos.setZ(pos.getZ() - val);

			} else {
				ERR(("Not supported rule! %s", r.c_str()));
			}
		}

		PRINTF(("PART_A: %d", pos.getX() * pos.getZ()));
	}

	{
		Point3d<int64_t> pos;

		for (auto &r : lines) {
			int val;

			if (sscanf(r.c_str(), "forward %d", &val) == 1) {
				pos.setX(pos.getX() + val);
				pos.setZ(pos.getZ() + pos.getY() * val);

			} else if (sscanf(r.c_str(), "down %d", &val) == 1) {
				pos.setY(pos.getY() + val);

			} else if (sscanf(r.c_str(), "up %d", &val) == 1) {
				pos.setY(pos.getY() - val);

			} else {
				ERR(("Not supported rule! %s", r.c_str()));
			}
		}

		PRINTF(("PART_B: %ld", pos.getX() * pos.getZ()));
	}

	return 0;
}
