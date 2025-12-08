#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static float _distance(const Point3d<int> &a, const Point3d<int> &b) {
	return
		sqrt(
			std::pow(a.x() - b.x(), 2) + 
			std::pow(a.y() - b.y(), 2) + 
			std::pow(a.z() - b.z(), 2)
		);
}

struct JunctionComparator {
	bool operator()(const std::pair<Point3d<int>, Point3d<int>> &a, const std::pair<Point3d<int>, Point3d<int>> &b) const {
		return _distance(a.first, a.second) < _distance(b.first, b.second);
	}
};

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Point3d<int>> junctionBoxes;
		std::set<std::pair<Point3d<int>, Point3d<int>>, JunctionComparator> distances;

		for (const auto &l : lines) {
			auto coords = utils::strTok(l, ',');

			junctionBoxes.push_back(Point3d<int>(
				std::stoi(coords[0]), std::stoi(coords[1]), std::stoi(coords[2]))
			);
		}

		for (int src = 0; src < junctionBoxes.size(); src++) {
			for (int dst = 0; dst < junctionBoxes.size(); dst++) {
				auto &srcJunction = junctionBoxes[src];
				auto &dstJunction = junctionBoxes[dst];

				if (src == dst) {
					continue;
				}

				distances.insert({ srcJunction, dstJunction });
			}
		}

		{
			std::vector<std::set<Point3d<int>>> circuits;

			int connected = 0;

			while (! distances.empty()) {	
				auto closest = distances.begin();

				std::vector<std::set<Point3d<int>>>::iterator srcSet = circuits.end();
				std::vector<std::set<Point3d<int>>>::iterator dstSet = circuits.end();

				for (auto c = circuits.begin(); c != circuits.end(); ++c) {
					if (c->find(closest->first) != c->end()) {
						srcSet = c;
					} 
					
					if (c->find(closest->second) != c->end()) {
						dstSet = c;
					}

					if (srcSet != circuits.end() && dstSet != circuits.end()) {
						break;
					}
				}

				if (srcSet == circuits.end() && dstSet == circuits.end()) {
					circuits.push_back({ closest->first, closest->second });

				} else if (srcSet != circuits.end() && dstSet == circuits.end()) {
					srcSet->insert(closest->second);

				} else if (srcSet == circuits.end() && dstSet != circuits.end()) {
					dstSet->insert(closest->first);

				} else {
					if (srcSet != dstSet) {
						srcSet->insert(dstSet->begin(), dstSet->end());
						circuits.erase(dstSet);
					}
				}

				if (++connected == 1000) {
					long long int partA = 1;

					std::sort(circuits.begin(), circuits.end(), [](const auto &a, const auto &b) {
						return a.size() < b.size();
					});

					for (int i = 0; i < 3; i++) {
						partA *= circuits[circuits.size() - i - 1].size();
					}

					PRINTF(("PART_A: %lld", partA));

				} else if (circuits.size() == 1 && circuits[0].size() == junctionBoxes.size()) {
					PRINTF(("PART_B: %lld", (long long int) closest->first.x() * closest->second.x()));
					break;
				}

				distances.erase(closest);
			}
		}
	}

	return 0;
}