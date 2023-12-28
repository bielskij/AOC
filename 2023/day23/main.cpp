#include <stack>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Edge {
	Point<int> dst;
	int weight;

	friend bool operator<(const Edge &lhs, const Edge &rhs) {
		return lhs.dst < rhs.dst;
	}
};


int solve(std::vector<std::string> &map, bool ignoreSlopes) {
	Point<int> start;
	Point<int> end;

	start.y(0);
	start.x(map[0].find_first_of('.'));

	end.y(map.size() - 1);
	end.x(map.rbegin()->find_first_of('.'));

	std::map<Point<int>, std::set<Edge>> edges;

	// Build graph
	{
		for (int y = 0; y < map.size(); y++) {
			for (int x = 0; x < map[0].length(); x++) {
				char currentChar = map[y][x];

				std::vector<Point<int>> dirMod;

				if (currentChar == '#') {
					continue;
				}

				if (ignoreSlopes || currentChar == '.') {
					dirMod = {
						{ 0, 1 }, { 0, -1 }, { 1, 0 }, { -1 , 0 }
					};

				} else {
					switch (currentChar) {
						case '^':
							dirMod = {{ 0, -1 }};
							break;

						case 'v':
							dirMod = {{ 0, 1 }};
							break;

						case '<':
							dirMod = {{ -1, 0 }};
							break;

						case '>':
							dirMod = {{ 1, 0 }};
							break;

						default:
							break;
					}
				}

				for (auto &dm : dirMod) {
					Point<int> src = { x, y };
					Point<int> dst = src + dm;

					if (
						dst.y() >= 0 && dst.y() < map.size() &&
						dst.x() >= 0 && dst.x() < map[0].length() &&
						map[dst.y()][dst.x()] != '#'
					) {
						edges[src].insert({ dst, 1 });

						if (ignoreSlopes || map[dst.y()][dst.x()] == '.') {
							edges[dst].insert({ src, 1 });
						}
					}
				}
			}
		}
	}

	// Reduce graph
	if (ignoreSlopes) {
		auto it = edges.begin();
		while (it != edges.end()) {
			if (it->second.size() == 2) {
				auto &l = *it->second.begin();
				auto &r = *it->second.rbegin();

				edges[l.dst].erase({ it->first, 0 });
				edges[l.dst].insert({ r.dst, l.weight + r.weight });

				edges[r.dst].erase({ it->first, 0 });
				edges[r.dst].insert({ l.dst, l.weight + r.weight });

				it = edges.erase(it);

			} else {
				++it;
			}
		}
	}

	// Traverse
	{
		std::stack<std::pair<Point<int>, int>> stack;
		std::set<Point<int>> visited;

		int longest = 0;

		stack.push({ start, 0 });

		while (! stack.empty()) {
			auto p = stack.top(); stack.pop();

			if (p.second < 0) {
				visited.erase(p.first);
				continue;
			}

			if (p.first == end) {
				longest = std::max(longest, p.second);
				continue;
			}

			if (visited.find(p.first) != visited.end()) {
				continue;
			}

			visited.insert(p.first);

			stack.push({ p.first, -1 });

			for (auto &edge : edges[p.first]) {
				stack.push({ edge.dst, p.second + edge.weight });
			}
		}

		return longest;
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, false)));
	PRINTF(("PART_B: %d", solve(lines, true)));

	return 0;
}
