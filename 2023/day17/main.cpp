#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#include "utils/graph2.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

enum Direction {
	N, E, S, W
};

struct ID {
	Direction  direction;
	Point<int> position;
	int        steps;

	ID() {
		this->direction = Direction::N;
		this->steps     = 0;
	}

	ID(Direction d, const Point<int> &p, int steps) : position(p) {
		this->direction = d;
		this->steps     = steps;
	}

	bool operator==(const ID &other) {
		return std::tie(position, direction, steps) == std::tie(other.position, other.direction, other.steps);
	}

	bool operator!=(const ID &other) {
		return ! (*this == other);
	}

	friend bool operator<(const ID &a, const ID &b) {
		return std::tie(a.position, a.direction, a.steps) < std::tie(b.position, b.direction, b.steps);
	}
};


struct StructureProvider : public graph::GraphStructureProvider<ID> {
	static const std::array<Point<int>, 4> dirMod;

	StructureProvider(const std::vector<std::string> &m, int minSteps, int maxSteps) : m(m) {
		this->minSteps = minSteps;
		this->maxSteps = maxSteps;
	}

	float getCost(const Point<int> &p) const {
		return this->m[p.y()][p.x()] - '0';
	}

	std::vector<graph::Edge<ID>> getEdges(const ID &ctx) const override {
		std::vector<graph::Edge<ID>> ret;

		if ((ctx.position.x() == 0) && (ctx.position.y() == 0)) {
			ret.push_back(
				graph::Edge<ID>(
					ctx,
					ID(Direction::E, { 1, 0 }, 1),
					this->getCost({ 1, 0 })
				)
			);

			ret.push_back(
				graph::Edge<ID>(
					ctx,
					ID(Direction::S, { 0, 1 }, 1),
					this->getCost({ 0, 1 })
				)
			);

		} else {
			Direction reverseDir;

			switch (ctx.direction) {
				case Direction::N: reverseDir = Direction::S; break;
				case Direction::E: reverseDir = Direction::W; break;
				case Direction::S: reverseDir = Direction::N; break;
				case Direction::W: reverseDir = Direction::E; break;
			}

			for (auto d : { Direction::N, Direction::E, Direction::S, Direction::W }) {
				if (d == reverseDir) {
					continue;
				}

				Point<int> nextPoint = ctx.position + dirMod.at(d);

				if (
					(nextPoint.x() >= 0) && (nextPoint.x() < m[0].length()) &&
					(nextPoint.y() >= 0) && (nextPoint.y() < m.size())
				) {
					if (ctx.direction == d) {
						if (ctx.steps < maxSteps) {
							ret.push_back(
								graph::Edge<ID>(
									ctx,
									ID(ctx.direction, nextPoint, ctx.steps + 1),
									getCost(nextPoint)
								)
							);
						}

					} else {
						if (ctx.steps >= minSteps) {
							ret.push_back(
								graph::Edge<ID>(
									ctx,
									ID(d, nextPoint, 1),
									getCost(nextPoint)
								)
							);
						}
					}
				}
			}
		}

		return ret;
	}

	ID getStart() {
		return ID(Direction::S, { 0, 0 }, 0);
	}

	const std::vector<std::string> &m;
	int minSteps;
	int maxSteps;
};


const std::array<Point<int>, 4> StructureProvider::dirMod = {
	Point<int>(0, -1), Point<int>(1, 0), Point<int>(0, 1), Point<int>(-1, 0)
};


const int solve(const std::vector<std::string> &map, int minSteps, int maxSteps) {
	StructureProvider provider(map, minSteps, maxSteps);

	graph::GraphSearchDijkstra<ID> searchAlgo;

	{
		searchAlgo.search(provider, provider.getStart());

		int partA = std::numeric_limits<int>::max();

		{
			auto endPos = Point<int>(map[0].length() - 1, map.size() - 1);

			std::vector<ID> possibleEndIDs;

			for (int i = minSteps; i <= maxSteps; i++) {
				possibleEndIDs.push_back(ID(Direction::S, endPos, i));
				possibleEndIDs.push_back(ID(Direction::E, endPos, i));
			}

			std::vector<ID> route;

			for (auto id : possibleEndIDs) {
				searchAlgo.getRoute(route, id);

				if (! route.empty()) {
					int res = 0;


					auto m = map;

					for (int i = 1; i < route.size(); i++) {
						auto &id = route[i];

						res += provider.getCost(id.position);
					}

					if (partA > res) {
						partA = res;
					}
				}
			}
		}

		return partA;
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, 1, 3)));
	PRINTF(("PART_B: %d", solve(lines, 4, 10)));

	return 0;
}
