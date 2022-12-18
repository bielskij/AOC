#include <cassert>
#include <deque>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Side {
	Point3d<int> start;
	Point3d<int> end;

	Side(const Point3d<int> &start, const Point3d<int> &end) : start(start), end(end) {

	}

	uint64_t getId() const {
		uint64_t ret = end.z();

		ret <<= 8; ret |= end.y();
		ret <<= 8; ret |= end.x();
		ret <<= 8; ret |= start.z();
		ret <<= 8; ret |= start.y();
		ret <<= 8; ret |= start.x();

		return ret;
	}
};


static uint32_t toId(const Point3d<int> p) {
	uint32_t ret = p.z();

	ret <<= 8; ret |= p.y();
	ret <<= 8; ret |= p.x();

	return ret;
}


std::vector<Point3d<int>> getNeighbours(const Point3d<int> &pos) {
	return {
		{ pos.x() + 1, pos.y()    , pos.z()    },
		{ pos.x() - 1, pos.y()    , pos.z()    },
		{ pos.x()    , pos.y()    , pos.z() - 1},
		{ pos.x()    , pos.y()    , pos.z() + 1},
		{ pos.x()    , pos.y() + 1, pos.z()    },
		{ pos.x()    , pos.y() - 1, pos.z()    }
	};
}


struct Cube {
	Cube() : Cube(Point3d<int>()) {

	}

	Cube(const Point3d<int> &pos) : pos(pos) {
		// front
		sides.push_back(Side(Point3d<int>(pos.x()    , pos.y()    , pos.z()    ), Point3d<int>(pos.x() + 1, pos.y() + 1, pos.z()    )));
		// top
		sides.push_back(Side(Point3d<int>(pos.x()    , pos.y() + 1, pos.z()    ), Point3d<int>(pos.x() + 1, pos.y() + 1, pos.z() - 1)));
		// back
		sides.push_back(Side(Point3d<int>(pos.x()    , pos.y()    , pos.z() - 1), Point3d<int>(pos.x() + 1, pos.y() + 1, pos.z() - 1)));
		// botton
		sides.push_back(Side(Point3d<int>(pos.x()    , pos.y()    , pos.z()    ), Point3d<int>(pos.x() + 1, pos.y()    , pos.z() - 1)));
		// right
		sides.push_back(Side(Point3d<int>(pos.x() + 1, pos.y()    , pos.z()    ), Point3d<int>(pos.x() + 1, pos.y() + 1, pos.z() - 1)));
		// left
		sides.push_back(Side(Point3d<int>(pos.x()    , pos.y()    , pos.z()    ), Point3d<int>(pos.x()    , pos.y() + 1, pos.z() - 1)));
	}

	uint32_t getId() const {
		return toId(pos);
	}

	Point3d<int>      pos;
	std::vector<Side> sides;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<uint32_t, Cube> cubes;

	for (auto &l : lines) {
		auto coords = utils::toIntV<int>(utils::strTok(l, ','));

		assert(coords.size() == 3);

		auto cube = Cube(Point3d<int>(coords[0], coords[1], coords[2]));

		cubes[cube.getId()] = cube;
	}

	{
		std::map<uint64_t, int> sides;

		for (auto &cube : cubes) {
			for (auto &side : cube.second.sides) {
				sides[side.getId()]++;
			}
		}

		int partA = 0;

		for (auto &s : sides) {
			if (s.second == 1) {
				partA++;
			}
		}

		PRINTF(("PART_A: %zd", partA));
	}

	{
		Point3d<int> min(std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
		Point3d<int> max(std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

		for (auto &cube : cubes) {
			for (int i = 0; i < 3; i++) {
				if (cube.second.pos.get(i) < min.get(i)) {
					min.set(i, cube.second.pos.get(i));
				}

				if (cube.second.pos.get(i) > max.get(i)) {
					max.set(i, cube.second.pos.get(i));
				}
			}
		}

		for (int i = 0; i < 3; i++) {
			min.set(i, min.get(i) - 1);
			max.set(i, max.get(i) + 1);
		}

		{
			std::set<uint32_t> visited;
			std::deque<Point3d<int>> queue;

			queue.push_back(min);
			visited.insert(toId(min));

			while (! queue.empty()) {
				Point3d<int> current = queue.front(); queue.pop_front();

				for (auto &n : getNeighbours(current)) {
					auto id = toId(n);

					bool valid = true;
					for (int i = 0; i < 3; i++) {
						if (n.get(i) < min.get(i) || n.get(i) > max.get(i)) {
							valid = false;
							break;
						}
					}

					if (! valid) {
						continue;
					}

					if (visited.find(id) != visited.end()) {
						continue;
					}

					if (cubes.find(id) != cubes.end()) {
						continue;
					}

					queue.push_back(n);
					visited.insert(id);
				}
			}

			{
				int result = 0;

				for (auto &c : cubes) {
					for (auto &n : getNeighbours(c.second.pos)) {
						if (visited.find(toId(n)) != visited.end()) {
							result++;
						}
					}
				}

				PRINTF(("PART_B: %d", result));
			}
		}
	}

	return 0;
}
