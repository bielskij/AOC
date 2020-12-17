#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


typedef std::map<int, std::map<int, std::map<int, int>>>                Map3D;
typedef std::map<int, std::map<int, std::map<int, std::map<int, int>>>> Map4D;

template <class T>
class Point4d : public Point3d<T> {
	public:
		Point4d(int x, int y, int z, int t) : Point3d<T>(x, y, z) {
			this->_t = t;
		}

		T getT() const {
			return this->_t;
		}

	private:
		int _t;
};


std::vector<Point3d<int>> getNeighbours(int x, int y, int z) {
	std::vector<Point3d<int>> ret;

	int delta[] = { -1, 0, 1 };

	for (auto dx : delta) {
		for (auto dy : delta) {
			for (auto dz : delta) {

				if (dx == 0 && dy == 0 && dz == 0) {
					continue;
				}

				ret.push_back(Point3d<int>(x + dx, y + dy, z + dz));
			}
		}
	}

	return ret;
}


std::vector<Point4d<int>> getNeighbours(int x, int y, int z, int t) {
	std::vector<Point4d<int>> ret;

	int delta[] = { -1, 0, 1 };

	for (auto dx : delta) {
		for (auto dy : delta) {
			for (auto dz : delta) {
				for (auto dt : delta) {

					if (dx == 0 && dy == 0 && dz == 0 && dt == 0) {
						continue;
					}

					ret.push_back(Point4d<int>(x + dx, y + dy, z + dz, t + dt));
				}
			}
		}
	}

	return ret;
}


static int getActive(Map3D &map, Point3d<int> &p) {
	auto xIt = map.find(p.getX());
	if (xIt != map.end()) {
		auto yIt = xIt->second.find(p.getY());
		if (yIt != xIt->second.end()) {
			if (yIt->second.find(p.getZ()) != yIt->second.end()) {
				return map[p.getX()][p.getY()][p.getZ()];
			}
		}
	}

	return 0;
}


static int getActive(Map4D &map, Point4d<int> &p) {
	auto xIt = map.find(p.getX());
	if (xIt != map.end()) {
		auto yIt = xIt->second.find(p.getY());
		if (yIt != xIt->second.end()) {
			auto zIt = yIt->second.find(p.getZ());
			if (zIt != yIt->second.end()) {
				if (zIt->second.find(p.getT()) != zIt->second.end()) {
					return map[p.getX()][p.getY()][p.getZ()][p.getT()];
				}
			}
		}
	}

	return 0;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		Map3D map;

		for (int y = 0; y < lines.size(); y++) {
			std::string &line = lines[y];

			for (int x = 0; x < line.size(); x++) {
				map[x][y][0] = line[x] == '#' ? 1 : 0;
			}
		}

		for (int round = 0; round < 6; round++) {
			Map3D newMap;

			for (auto &mx : map) {
				for (auto &my : mx.second) {
					for (auto &mz : my.second) {
						auto neighbours = getNeighbours(mx.first, my.first, mz.first);

						// 1 step forward
						for (auto &n : neighbours) {
							auto neighboursForward = getNeighbours(n.getX(), n.getY(), n.getZ());

							int active = 0;
							for (auto &n2 : neighboursForward) {
								active += getActive(map, n2);
							}

							int field = getActive(map, n);
							if (field) {
								if (active < 2 || active > 3) {
									field = 0;
								}

							} else {
								if (active == 3) {
									field = 1;
								}
							}

							newMap[n.getX()][n.getY()][n.getZ()] = field;
						}
					}
				}
			}

			map = newMap;
		}

		{
			int partA = 0;

			for (auto &mx : map) {
				for (auto &my : mx.second) {
					for (auto &mz : my.second) {
						partA += mz.second;
					}
				}
			}

			PRINTF(("PART_A: %d", partA));
		}
	}

	{
		Map4D map;

		for (int y = 0; y < lines.size(); y++) {
			std::string &line = lines[y];

			for (int x = 0; x < line.size(); x++) {
				map[x][y][0][0] = line[x] == '#' ? 1 : 0;
			}
		}

		for (int round = 0; round < 6; round++) {
			Map4D newMap;

			for (auto &mx : map) {
				for (auto &my : mx.second) {
					for (auto &mz : my.second) {
						for (auto &mt : mz.second) {
							auto neighbours = getNeighbours(mx.first, my.first, mz.first, mt.first);

							// 1 step forward
							for (auto &n : neighbours) {
								auto neighboursForward = getNeighbours(n.getX(), n.getY(), n.getZ(), n.getT());

								int active = 0;
								for (auto &n2 : neighboursForward) {
									active += getActive(map, n2);
								}

								int field = getActive(map, n);
								if (field) {
									if (active < 2 || active > 3) {
										field = 0;
									}

								} else {
									if (active == 3) {
										field = 1;
									}
								}

								newMap[n.getX()][n.getY()][n.getZ()][n.getT()] = field;
							}
						}
					}
				}
			}

			map = newMap;
		}

		{
			int partB = 0;

			for (auto &mx : map) {
				for (auto &my : mx.second) {
					for (auto &mz : my.second) {
						for (auto &mt : mz.second) {
							partB += mt.second;
						}
					}
				}
			}

			PRINTF(("PART_B: %d", partB));
		}
	}

	return 0;
}
