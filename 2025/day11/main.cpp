#include <deque>
#include <stack>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int64_t _solve(std::map<int, std::vector<int>> &relations, int current, int out, int fft, int dac, bool seenFft, bool seenDac, std::map<uint16_t, int64_t> &cache) {
    seenFft = seenFft || (current == fft);
    seenDac = seenDac || (current == dac);
    
    if (current == out) {
        return (int64_t)(seenFft && seenDac ? 1 : 0);
    }

    uint16_t state = current | (seenFft << 15) | (seenDac << 14);
    if (cache.count(state)) {
        return cache[state];
    }

    int64_t sum = 0;
    for (auto next : relations[current]) {
        sum += _solve(relations, next, out, fft, dac, seenFft, seenDac, cache);
    }

    cache[state] = sum;

    return sum;
};

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

    std::map<std::string, std::vector<std::string>> stringRelations;
    std::map<std::string, int> string2IntMap;
    std::map<int, std::vector<int>> intRelations;

    std::set<std::string> allNodes;

    for (const auto &l : lines) {
        auto toks = utils::strTok(l, ' ');

        auto srcNode = toks[0].substr(0, toks[0].length() - 1);

        allNodes.insert(srcNode);

        for (int i = 1; i < toks.size(); i++) {
            stringRelations[srcNode].push_back(toks[i]);
            allNodes.insert(toks[i]);
        }
    }

    {
        int nodeId = 0;

        for (const auto &node : allNodes) {
            string2IntMap[node] = nodeId;

            nodeId++;
        }

        for (const auto &relation : stringRelations) {
            auto &src = intRelations[string2IntMap[relation.first]];

            for (const auto &dst : relation.second) {
                src.push_back(string2IntMap[dst]);
            }
        }
    }

    {
        std::map<uint16_t, int64_t> cache;

        PRINTF(("PART_A: %" PRId64, 
            _solve(intRelations, string2IntMap["you"], string2IntMap["out"], string2IntMap["fft"], string2IntMap["dac"], true, true, cache)
        ));
    }

    {
        std::map<uint16_t, int64_t> cache;

        PRINTF(("PART_B: %" PRId64, 
            _solve(intRelations, string2IntMap["svr"], string2IntMap["out"], string2IntMap["fft"], string2IntMap["dac"], false, false, cache)
        ));
    }

	return 0;
}
