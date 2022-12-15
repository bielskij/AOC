#include <cassert>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Sensor {
	Point<int> position;
	Point<int> nearestBeacon;

	int radius;

	void set(int pX, int pY, int bX, int bY) {
		this->position.x(pX);
		this->position.y(pY);

		this->nearestBeacon.x(bX);
		this->nearestBeacon.y(bY);

		this->radius = utils::manhattanDistance(position, nearestBeacon);
	}
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<Sensor> sensors;
	std::set<Point<int>> beacons;

	for (auto &l : lines) {
		Sensor s;

		int x1, y1, x2, y2;

		if (sscanf(l.c_str(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &x1, &y1, &x2, &y2) != 4) {
			assert(false);
		}

		s.set(x1, y1, x2, y2);

		beacons.insert(s.nearestBeacon);

		sensors.push_back(s);
	}

	{
		int row = 2000000;
		std::set<Point<int>> safePoints;
		for (auto &s : sensors) {
			Point<int> p(s.position.x(), row);

			if (utils::manhattanDistance(s.position, p) <= s.radius) {
				safePoints.insert(p);

				p = Point<int>(s.position.x() - 1, row);
				while (utils::manhattanDistance(p, s.position) <= s.radius) {
					safePoints.insert(p);
					p.decX();
				}

				for (int i = 0; i < s.position.x() - p.x() - 1; i++) {
					safePoints.insert(Point<int>(s.position.x() + i + 1, row));
				}
			}
		}

		for (auto b : beacons) {
			auto it = safePoints.find(b);
			if (it != safePoints.end()) {
				safePoints.erase(it);
			}
		}

		PRINTF(("PART_A: %zd", safePoints.size()));
	}

	{
		for (int x = 0; x < 4000000; x++) {
			for (int y = 0; y < 4000000; y++) {
				bool found = false;

				for (auto &s : sensors) {
					if (
						((s.position.x()      == x)  && (s.position.y()      == y)) ||
						((s.nearestBeacon.x() == x ) && (s.nearestBeacon.y() == y))
					) {
						found = true;
						break;
					}
				}

				if (! found) {
					bool possibleLocation = true;

					for (auto &s : sensors) {
						int dist = utils::manhattanDistance(s.position, Point<int>(x, y));

						if (dist <= s.radius) {
							possibleLocation = false;

							y = s.position.y() + s.radius - std::abs(s.position.x() - x);
							break;
						}
					}

					if (possibleLocation && x > 0) {
						PRINTF(("PART_B: %lld", (long long int) 4000000 * (long long int) x +  (long long int)y));

						exit(0);
					}
				}
			}
		}
	}

	return 0;
}
