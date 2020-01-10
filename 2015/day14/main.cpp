#include <jsoncpp/json/json.h>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Parameters {
	int speed;
	int flyTime;
	int restTime;

	int partA;
	int partB;

	int  time;
	bool fly;

	Parameters() {
		this->speed      = 0;
		this->flyTime    = 0;
		this->restTime   = 0;

		this->partA      = 0;
		this->partB      = 0;
		this->time       = 0;
		this->fly        = true;
	}

	Parameters(int speed, int fly, int rest) {
		this->speed    = speed;
		this->flyTime  = fly;
		this->restTime = rest;

		this->partA    = 0;
		this->partB    = 0;
		this->time     = 0;
		this->fly      = true;
	}
};


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	// Dancer can fly 27 km/s for 5 seconds, but then must rest for 132 seconds.
	std::map<std::string, Parameters> reindeers;

	for (auto line : data) {
		char deer[32];
		Parameters params;

		if (sscanf(line.c_str(), "%s can fly %d km/s for %d seconds, but then must rest for %d seconds", deer, &params.speed, &params.flyTime, &params.restTime) == 4) {
			reindeers[deer] = params;

		} else {
			abort();
		}
	}


	std::vector<Parameters *> leads;
	int leadsScore;

	for (int t = 0; t < utils::toInt(argv[2]); t++) {
		leadsScore = 0;
		leads.clear();

		for (auto &r : reindeers) {
			r.second.time++;

			if (r.second.fly) {
				r.second.partA += r.second.speed;

				if (r.second.time == r.second.flyTime) {
					r.second.fly  = false;
					r.second.time = 0;
				}

			} else {
				if (r.second.time == r.second.restTime) {
					r.second.fly  = true;
					r.second.time = 0;
				}
			}

			if (r.second.partA > leadsScore) {
				leadsScore = r.second.partA;

				leads.clear();
				leads.push_back(&r.second);

			} else if (r.second.partA == leadsScore) {
				leads.push_back(&r.second);
			}
		}

		for (auto &l : leads) {
			l->partB++;
		}
	}
	{
		int maxPartA = 0;
		int maxPartB = 0;

		for (auto &r : reindeers) {
			if (r.second.partA > maxPartA) {
				maxPartA = r.second.partA;
			}

			if (r.second.partB > maxPartB) {
				maxPartB = r.second.partB;
			}
		}


		PRINTF(("PART_A: %d", maxPartA));
		PRINTF(("PART_B: %d", maxPartB));
	}
}
