#include <queue>
#include <functional>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Scanner {
	int          id;
	Point3d<int> position;

	std::vector<Point3d<int>> beacons;
};


using RotateMethod = std::function<Point3d<int>(const Point3d<int> &src)>;


static const std::vector<RotateMethod> rotates = {
	[](const Point3d<int> &src) { return Point3d<int>(src.x(),  src.y(),  src.z()); },  // x,  y,  z
	[](const Point3d<int> &src) { return Point3d<int>(src.x(), -src.z(),  src.y()); },  // x, -z,  y
	[](const Point3d<int> &src) { return Point3d<int>(src.x(), -src.y(), -src.z()); },  // x, -y, -z
	[](const Point3d<int> &src) { return Point3d<int>(src.x(),  src.z(), -src.y()); },  // x,  z, -y

	[](const Point3d<int> &src) { return Point3d<int>(-src.x(), -src.y(),  src.z()); }, // -x, -y,  z
	[](const Point3d<int> &src) { return Point3d<int>(-src.x(), -src.z(), -src.y()); }, // -x, -z, -y
	[](const Point3d<int> &src) { return Point3d<int>(-src.x(),  src.y(), -src.z()); }, // -x,  y, -z
	[](const Point3d<int> &src) { return Point3d<int>(-src.x(),  src.z(),  src.y()); }, // -x,  z,  y

	[](const Point3d<int> &src) { return Point3d<int>(-src.y(), src.x(),  src.z()); },  // -y, x,  z
	[](const Point3d<int> &src) { return Point3d<int>( src.z(), src.x(),  src.y()); },  //  z, x,  y
	[](const Point3d<int> &src) { return Point3d<int>( src.y(), src.x(), -src.z()); },  //  y, x, -z
	[](const Point3d<int> &src) { return Point3d<int>(-src.z(), src.x(), -src.y()); },  // -z, x, -y

	[](const Point3d<int> &src) { return Point3d<int>( src.y(), -src.x(),  src.z()); }, //  y, -x,  z
	[](const Point3d<int> &src) { return Point3d<int>( src.z(), -src.x(), -src.y()); }, //  z, -x, -y
	[](const Point3d<int> &src) { return Point3d<int>(-src.y(), -src.x(), -src.z()); }, // -y, -x, -z
	[](const Point3d<int> &src) { return Point3d<int>(-src.z(), -src.x(),  src.y()); }, // -z, -x,  y

	[](const Point3d<int> &src) { return Point3d<int>(-src.z(),  src.y(), src.x()); },  // -z,  y, x
	[](const Point3d<int> &src) { return Point3d<int>( src.y(),  src.z(), src.x()); },  //  y,  z, x
	[](const Point3d<int> &src) { return Point3d<int>( src.z(), -src.y(), src.x()); },  //  z, -y, x
	[](const Point3d<int> &src) { return Point3d<int>(-src.y(), -src.z(), src.x()); },  // -y, -z, x

	[](const Point3d<int> &src) { return Point3d<int>(-src.z(), -src.y(), -src.x()); }, // -z, -y, -x
	[](const Point3d<int> &src) { return Point3d<int>(-src.y(),  src.z(), -src.x()); }, // -y,  z, -x
	[](const Point3d<int> &src) { return Point3d<int>( src.z(),  src.y(), -src.x()); }, //  z,  y, -x
	[](const Point3d<int> &src) { return Point3d<int>( src.y(), -src.z(), -src.x()); }  //  y, -z, -x

};


static std::map<Point3d<int>, Point3d<int>> genVectorsFromBeacons(const std::set<Point3d<int>> &beacons) {
	std::map<Point3d<int>, Point3d<int>> ret;

	for (const auto &b1 : beacons) {
		for (const auto &b2 : beacons) {
			if (b1 != b2) {
				ret[b2.vector(b1)] = b2;
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<Scanner> scanners;

	{
		Scanner currentScanner;

		for (const auto &l : lines) {
			if (l.empty()) {
				scanners.push_back(currentScanner);

				currentScanner = Scanner();

			} else {
				int a, b, c;

				if (sscanf(l.c_str(), "%d,%d,%d", &a, &b, &c) == 3) {
					currentScanner.beacons.push_back(Point3d<int>(a, b, c));

				} else if (sscanf(l.c_str(), "--- scanner %d ---", &a) == 1) {
					currentScanner.id = a;
				}
			}
		}

		if (! currentScanner.beacons.empty()) {
			scanners.push_back(currentScanner);
		}
	}

	{
		std::vector<Scanner>   orderedScanners;
		std::set<Point3d<int>> orderedBeacons;

		std::map<Point3d<int>, Point3d<int>> orderedVectors;

		std::queue<Scanner> scannersToCheck;

		// First scanner is a base in point 0, 0, 0.
		{
			orderedScanners.push_back(scanners[0]);

			for (const auto &beacon : scanners[0].beacons) {
				orderedBeacons.insert(beacon);
			}

			orderedVectors = genVectorsFromBeacons(orderedBeacons);
		}

		for (int i = 1; i < scanners.size(); i++) {
			scannersToCheck.push(scanners[i]);
		}

		while (! scannersToCheck.empty()) {
			Scanner scanner = scannersToCheck.front(); scannersToCheck.pop();

			RotateMethod rotateMethod = nullptr;
			Point3d<int> rotateTrans;

			for (auto rotate : rotates) {
				for (const auto &beaconSrc : scanner.beacons) {
					int matchCount = 0;

					auto rotatedSrcBeacon = rotate(beaconSrc);

					for (const auto &beaconDst : scanner.beacons) {
						Point3d<int> rotatedDstBeacon = rotate(beaconDst);

						// Skip vectors to self
						if (beaconSrc == beaconDst) {
							continue;
						}

						auto vector = rotatedSrcBeacon.vector(rotatedDstBeacon);

						if (orderedVectors.find(vector) != orderedVectors.end()) {
							matchCount++;

							if (matchCount >= 11) {
								rotateMethod = rotate;
								rotateTrans  = rotatedSrcBeacon.vector(orderedVectors[vector]);
								break;
							}
						}
					}

					if (rotateMethod != nullptr) {
						break;
					}
				}

				if (rotateMethod != nullptr) {
					break;
				}
			}

			if (rotateMethod != nullptr) {
				std::vector<Point3d<int>> rotatedBeacons;
				std::vector<Point3d<int>> translatedBeacons;

				for (const auto &beacon : scanner.beacons) {
					rotatedBeacons.push_back(rotateMethod(beacon));
				}

				for (const auto &beacon : rotatedBeacons) {
					translatedBeacons.push_back(Point3d<int>(
						beacon.x() + rotateTrans.x(),
						beacon.y() + rotateTrans.y(),
						beacon.z() + rotateTrans.z()
					));
				}

				for (const auto &beacon : translatedBeacons) {
					orderedBeacons.insert(beacon);
				}

				// Update vector list
				orderedVectors = genVectorsFromBeacons(orderedBeacons);

				scanner.position = rotateTrans;

				orderedScanners.push_back(scanner);

			} else {
				scannersToCheck.push(scanner);
			}
		}

		PRINTF(("PART_A: %zd", orderedBeacons.size()));

		{
			int maxDistance = 0;

			for (const auto &from : orderedScanners) {
				for (const auto &to : orderedScanners) {
					int distance = utils::manhattanDistance(from.position, to.position);

					if (distance > maxDistance) {
						maxDistance = distance;
					}
				}
			}

			PRINTF(("PART_B: %d", maxDistance));
		}
	}

	return 0;
}
