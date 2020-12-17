#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Rule {
	std::string name;

	std::pair<int, int> ranges[2];
};


enum Mode {
	MODE_RULE,
	MODE_TICKET,
	MODE_NEARBY_TICKET
};


static bool isValidByRule(int val, const Rule &rule) {
	if (
		(val >= rule.ranges[0].first && val <= rule.ranges[0].second) ||
		(val >= rule.ranges[1].first && val <= rule.ranges[1].second)
	) {
		return true;
	}

	return false;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<std::string, Rule> rules;
		std::vector<int64_t> ticket;
		std::vector<std::vector<int64_t>> nearbyTickets;

		{
			Mode mode = MODE_RULE;

			for (auto &line : lines) {
				if (line.empty()) {
					continue;
				}

				switch (mode) {
					case MODE_RULE:
						{
							Rule rule;
							char name[64];

							if (sscanf(line.c_str(), "%[^:]: %d-%d or %d-%d", name, &rule.ranges[0].first, &rule.ranges[0].second, &rule.ranges[1].first, &rule.ranges[1].second) == 5) {
								rule.name = name;

								rules[name] = rule;

							} else {
								if (line == "your ticket:") {
									mode = MODE_TICKET;
								}
							}
						}
						break;

					case MODE_NEARBY_TICKET:
						{
							nearbyTickets.push_back(utils::toInt64tV(utils::strTok(line, ',')));
						}
						break;

					case MODE_TICKET:
						{
							if (line == "nearby tickets:") {
								mode = MODE_NEARBY_TICKET;

							} else {
								ticket = utils::toInt64tV(utils::strTok(line, ','));
							}
						}
						break;
				}
			}

			int partA = 0;

			std::vector<std::vector<int64_t>> validTickets;

			for (auto &nearby : nearbyTickets) {
				bool validTicket = true;

				for (auto field : nearby) {
					bool validField = false;

					for (auto &rule : rules) {
						if (isValidByRule(field, rule.second)) {
							validField = true;
							break;
						}
					}

					if (! validField) {
						partA += field;

						validTicket = false;
					}
				}

				if (validTicket) {
					validTickets.push_back(nearby);
				}
			}

			PRINTF(("PART_A: %d", partA));

			{
				std::map<int, std::vector<std::string>> mapCandidates;

				for (auto &rule : rules) {
					for (int col = 0; col < ticket.size(); col++) {
						bool allValid = true;

						for (auto &ticket : validTickets) {
							if (! isValidByRule(ticket[col], rule.second)) {
								allValid = false;
								break;
							}
						}

						if (allValid) {
							mapCandidates[col].push_back(rule.first);
						}
					}
				}

				std::map<int, std::string> map;

				while (! mapCandidates.empty()) {
					for (auto it = mapCandidates.begin(); it != mapCandidates.end(); it++) {
						if (it->second.size() == 1) {
							map[it->first] = it->second[0];

							for (auto &entry : mapCandidates) {
								auto toEraseIt = std::find(entry.second.begin(), entry.second.end(), map[it->first]);

								if (toEraseIt != entry.second.end()) {
									entry.second.erase(toEraseIt);
								}
							}

							mapCandidates.erase(it);
							break;
						}
					}
				}

				int64_t partB = 1;
				for (int col = 0; col < ticket.size(); col++) {
					if (map[col].compare(0, 9, "departure") == 0) {
						partB *= ticket[col];
					}
				}

				PRINTF(("PART_B: %" PRId64, partB));
			}
		}
	}

	return 0;
}
