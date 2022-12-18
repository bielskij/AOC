#include <cassert>
#include <deque>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#include "utils/graph/Graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Valve {
	std::string              id;
	int                      rate;
	std::vector<std::string> tunnels;

	Valve() {
		rate = 0;
	}

	Valve(const std::string &id, int rate) : id(id) {
		this->rate = rate;
	}
};


struct State {
	Valve            *valve;
	int               minutes;
	std::set<Valve *> opened;
	int               elephants;

	bool operator<(const State & other) const {
		return
			std::tie(valve, minutes, opened, elephants) <
			std::tie(other.valve, other.minutes, other.opened, other.elephants);
	}
};


static int solve(Valve *start, int minutes, std::set<Valve *> opened, std::map<std::string, Valve> &valves, std::map<State, int> &cache, int elephantNo) {
	if (minutes == 0) {
		if (elephantNo) {
			return solve(&valves["AA"], 26, opened, valves, cache, elephantNo - 1);
		}

		return 0;
	}

	State s{ start, minutes, opened, elephantNo };

	auto cacheIt = cache.find(s);
	if (cacheIt != cache.end()) {
		return cacheIt->second;
	}

	int max = 0;
	if (start->rate > 0 && (opened.find(start) == opened.end())) {
		opened.insert(start);

		int val = (minutes - 1) * start->rate + solve(start, minutes - 1, opened, valves, cache, elephantNo);

		opened.erase(start);

		max = val;
	}

	for (auto &n : start->tunnels) {
		Valve *neighbour = &valves[n];

		max = std::max(max, solve(neighbour, minutes - 1, opened, valves, cache, elephantNo));
	}

	cache[s] = max;

	return max;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<std::string, Valve> valves;

	for (auto &line : lines) {
		auto toks = utils::strTok(line, ';');

		assert(toks.size() == 2);

		{
			char valveId[8];
			int rate;

			assert(sscanf(utils::trim(toks[0], " ").c_str(), "Valve %s has flow rate=%d", valveId, &rate) == 2);

			valves[valveId] = Valve(valveId, rate);

			{
				auto tunnels = utils::strTok(utils::trim(toks[1].substr(std::string("tunnels lead to valves").size() + 1), " "), ',');

				for (auto &t : tunnels) {
					valves[valveId].tunnels.push_back(utils::trim(t, " "));
				}
			}
		}
	}

	{
		std::map<State, int> cache;

		PRINTF(("PART_A: %d", solve(&valves["AA"], 30, {}, valves, cache, 0)));
	}

	{
		std::map<State, int> cache;

		PRINTF(("PART_B: %d", solve(&valves["AA"], 26, {}, valves, cache, 1)));
	}

	return 0;
}
