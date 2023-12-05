#include <stack>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct SourceDestinationMap {
	int64_t sourceStart;
	int64_t sourceEnd;
	int64_t destinationStart;
	int64_t destinationEnd;
	int64_t range;

	SourceDestinationMap(int64_t startSource, int64_t startDestination, int64_t range) {
		this->destinationStart = startDestination;
		this->sourceStart      = startSource;
		this->range            = range;

		this->destinationEnd = this->destinationStart + this->range - 1;
		this->sourceEnd      = this->sourceStart      + this->range - 1;
	}

	int64_t inline getDestination(int64_t src) const {
		return this->destinationStart + (src - this->sourceStart);
	}
};


std::vector<std::pair<int64_t, int64_t>> solve(
	const std::vector<std::pair<int64_t, int64_t>> &seed,
	const std::map<std::string, std::map<std::string, std::vector<SourceDestinationMap>>> &maps
) {
	static const std::vector<std::pair<std::string, std::string>> transitions = {
		{ "seed",        "soil" },
		{ "soil",        "fertilizer" },
		{ "fertilizer",  "water" },
		{ "water",       "light" },
		{ "light",       "temperature" },
		{ "temperature", "humidity" },
		{ "humidity",    "location" }
	};

	{
		std::vector<std::pair<int64_t, int64_t>> prevStep;
		std::vector<std::pair<int64_t, int64_t>> nextStep = seed;

		for (const auto &transition : transitions) {
			const auto &transitionMap = maps.find(transition.first)->second.find(transition.second)->second;

			prevStep = nextStep;
			nextStep.clear();

			for (auto &prevRange : prevStep) {
				std::vector<std::pair<int64_t, int64_t>> newRanges;

				newRanges.reserve(transitionMap.size() * 2);

				newRanges.push_back(prevRange);

				for (auto &map : transitionMap) {
					auto it = newRanges.begin();

					while (it != newRanges.end()) {
						bool consumed = false;

						// --     (r)
						//   **** (m)
						if (it->first < map.sourceStart) {
							// --- (r)
							//   * (m)
							if (it->second >= map.sourceStart) {
								newRanges.push_back({
									it->first,
									map.sourceStart - 1
								});

								// ----------[-] (r)
								//        *****  (m)
								if (it->second <= map.sourceEnd) {
									nextStep.push_back({
										map.getDestination(map.sourceStart),
										map.getDestination(it->second)
									});

								// --------------- (r)
								//        *****    (m)
								} else {
									nextStep.push_back({
										map.getDestination(map.sourceStart),
										map.getDestination(map.sourceEnd)
									});

									newRanges.push_back({
										map.sourceEnd + 1,
										it->second
									});
								}

								consumed = true;
							}

						// -   (r)
						// *** (m)
						} else {
							if (it->first <= map.sourceEnd) {
								//    -[---]
								// ****
								if (it->second > map.sourceEnd) {
									nextStep.push_back({
										map.getDestination(it->first),
										map.getDestination(map.sourceEnd)
									});

									newRanges.push_back({
										map.sourceEnd + 1,
										it->second
									});

								//    ----
								// ********
								} else {
									nextStep.push_back({
										map.getDestination(it->first),
										map.getDestination(it->second)
									});
								}

								consumed = true;
							}
						}

						if (consumed) {
							it = newRanges.erase(it);

						} else {
							++it;
						}
					}
				}

				if (! newRanges.empty()) {
					nextStep.insert(nextStep.end(), newRanges.begin(), newRanges.end());
				}
			}
		}

		return nextStep;
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<int64_t> seeds;

		std::map<std::string, std::map<std::string, std::vector<SourceDestinationMap>>> maps;

		std::vector<SourceDestinationMap> *current = nullptr;

		for (auto &l : lines) {
			l = utils::trim(l, " \r\n\t");

			if (l.empty()) {
				current = nullptr;
				continue;
			}

			if (l.compare(0, 7, "seeds: ") == 0) {
				for (const auto &num : utils::strTok(l.substr(7), ' ')) {
					seeds.push_back(std::stoll(num));
				}

			} else {
				if (! std::isdigit(l[0])) {
					auto desc = utils::strTok(utils::strTok(l, ' ')[0], '-');

					current = &maps[desc[0]][desc[2]];

				} else {
					auto ranges = utils::strTok(l, ' ');

					current->push_back(SourceDestinationMap(
						std::stoll(ranges[1]), std::stoll(ranges[0]), std::stoll(ranges[2]))
					);
				}
			}
		}

		{
			std::vector<std::pair<int64_t, int64_t>> seedSource;

			for (const auto &seed : seeds) {
				seedSource.push_back({ seed, seed });
			}

			auto result = solve(seedSource, maps);

			{
				int64_t min = std::numeric_limits<int64_t>::max();

				for (auto &r : result) {
					if (r.first < min) {
						min = r.first;
					}
				}

				PRINTF(("PART_A: %" PRId64, min));
			}
		}

		{
			std::vector<std::pair<int64_t, int64_t>> seedSource;

			for (int i = 0; i < seeds.size(); i += 2) {
				seedSource.push_back({ seeds[i], seeds[i] + seeds[i + 1] - 1 });
			}

			auto result = solve(seedSource, maps);

			{
				int64_t min = std::numeric_limits<int64_t>::max();

				for (auto &r : result) {
					if (r.first < min) {
						min = r.first;
					}
				}

				PRINTF(("PART_B: %" PRId64, min));
			}
		}
	}
}
