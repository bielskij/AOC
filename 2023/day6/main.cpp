#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int getDistance(int holdTime, int raceTime) {
	return holdTime * (raceTime - holdTime);
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<int> time;
		std::vector<int> distance;

		for (auto &n : utils::strTok(utils::strTok(lines[0], ':')[1], ' ')) {
			auto s = utils::trim(n, " ");

			if (s.empty()) {
				continue;
			}

			time.push_back(std::stoi(s));
		}

		for (auto &n : utils::strTok(utils::strTok(lines[1], ':')[1], ' ')) {
			auto s = utils::trim(n, " ");

			if (s.empty()) {
				continue;
			}

			distance.push_back(std::stoi(s));
		}

		int partA = 1;

		for (int race = 0; race < time.size(); race++) {
			int possibleWins = 0;

			for (int i = 1; i < time[race] - 1; i++) {
				if (getDistance(i, time[race]) > distance[race]) {
					possibleWins++;
				}
			}

			partA *= possibleWins;
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		// a = x * (b - x)
		// a < -x^2 + bx
		// 0 < -x^2 + bx - a

		int64_t time;
		int64_t distance;

		{
			std::string timeStr = utils::trim(utils::strTok(lines[0], ':')[1], " ");

			timeStr.erase(remove_if(timeStr.begin(), timeStr.end(), isspace), timeStr.end());

			time = std::stoll(timeStr);
		}

		{
			std::string distStr = utils::trim(utils::strTok(lines[1], ':')[1], " ");

			distStr.erase(remove_if(distStr.begin(), distStr.end(), isspace), distStr.end());

			distance = std::stoll(distStr);
		}

		float a = -1;
		float b = time;
		float c = -distance;

		float delta = b * b - 4 * a * c;

		float x1 = (-b - sqrt(delta)) / 2 * a;
		float x2 = (-b + sqrt(delta)) / 2 * a;

		PRINTF(("PART_B: %d", (int) std::abs(x2 - x1)));
	}

	return 0;
}
