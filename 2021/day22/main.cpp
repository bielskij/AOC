#include <cstring>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Cuboid {
	Point3d<int64_t> start;
	Point3d<int64_t> end;
	bool             isOn;
	int              sign;

	Cuboid() {
		this->isOn = false;
		this->sign = 1;
	}

	Cuboid(const Point3d<int64_t> &start, const Point3d<int64_t> &end, bool isOn) : start(start), end(end) {
		this->isOn = isOn;
		this->sign = 1;
	}

	Cuboid(const Point3d<int64_t> &start, const Point3d<int64_t> &end, bool isOn, int sign) : start(start), end(end) {
		this->isOn = isOn;
		this->sign = sign;
	}

	int64_t getVolume() const {
		return
			this->sign *
			(this->end.x() - this->start.x() + 1) *
			(this->end.y() - this->start.y() + 1) *
			(this->end.z() - this->start.z() + 1);
	}
};


static Cuboid intersection(const Cuboid &a, const Cuboid &b) {
	Point3d<int64_t> begin(
		std::max(a.start.x(), b.start.x()),
		std::max(a.start.y(), b.start.y()),
		std::max(a.start.z(), b.start.z())
	);

	Point3d<int64_t> end(
		std::max(std::min(a.end.x(), b.end.x()), begin.x() - 1),
		std::max(std::min(a.end.y(), b.end.y()), begin.y() - 1),
		std::max(std::min(a.end.z(), b.end.z()), begin.z() - 1)
	);

	return Cuboid(begin, end, true, -1 * a.sign * b.sign);
}


static int64_t solve(std::vector<Cuboid> &cuboids, bool partA) {
	int64_t ret = 0;

	{
		std::vector<Cuboid> onCuboids;

		for (auto cuboidA : cuboids) {
			if (
				partA &&
				(
					((cuboidA.start.x() < -50) || (cuboidA.end.x() > 50)) ||
					((cuboidA.start.y() < -50) || (cuboidA.end.y() > 50)) ||
					((cuboidA.start.z() < -50) || (cuboidA.end.z() > 50))
				)
			) {
				continue;
			}

			{
				int maxIdx = onCuboids.size();

				for (int i = 0; i < maxIdx; i++) {
					auto intersectionCuboid = intersection(cuboidA, onCuboids[i]);
					auto intersectionVolume = intersectionCuboid.getVolume();

					if (intersectionVolume != 0) {
						ret += intersectionVolume;

						onCuboids.push_back(intersectionCuboid);
					}
				}
			}

			if (cuboidA.isOn) {
				ret += cuboidA.getVolume();

				onCuboids.push_back(cuboidA);
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Cuboid> cuboids;

		for (const auto &l : lines) {
			int x[2];
			int y[2];
			int z[2];
			char value[4];

			if (sscanf(l.c_str(), "%s x=%d..%d,y=%d..%d,z=%d..%d",
				value, &x[0], &x[1], &y[0], &y[1], &z[0], &z[1]
			) != 7) {
				PRINTF(("Parse failed!"));
				abort();

			} else {
				cuboids.push_back(Cuboid(Point3d<int64_t>(x[0], y[0], z[0]), Point3d<int64_t>(x[1], y[1], z[1]), strcmp(value, "off") != 0));
			}
		}

		PRINTF(("PART_A: %ld", solve(cuboids, true)));
		PRINTF(("PART_B: %ld", solve(cuboids, false)));
	}

	return 0;
}
