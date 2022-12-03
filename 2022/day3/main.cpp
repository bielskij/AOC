#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int getResult(const std::vector<std::string> &rows) {
	int ret = 0;

	{
		std::set<char> unique(rows[0].begin(), rows[0].end());

		std::map<char, int> freq;

		for (size_t i = 1; i < rows.size(); i++) {
			auto &l = rows[i];

			for (auto c : unique) {
				if (l.find(c) != std::string::npos) {
					freq[c]++;
				}
			}
		}

		for (auto p : freq) {
			if (p.second == rows.size() - 1) {
				if (std::islower(p.first)) {
					ret += (p.first - 'a' + 1);

				} else {
					ret += (p.first - 'A' + 27);
				}
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int sum = 0;

		for (auto &l : lines) {
			sum += getResult({ l.substr(0, l.length() / 2), l.substr(l.length() / 2) });
		}

		PRINTF(("PART_A: %d", sum));
	}

	{
		int sum = 0;

		for (size_t i = 0; i < lines.size(); i += 3) {
			std::vector<std::string> data;

			data.push_back(lines[i]);
			data.push_back(lines[i + 1]);
			data.push_back(lines[i + 2]);

			sum += getResult(data);
		}

		PRINTF(("PART_B: %d", sum));
	}

	return 0;
}
