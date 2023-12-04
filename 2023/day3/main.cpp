#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		auto parseNum = [](const std::string &s, int x, bool right, int &result) {
			std::string num;

			int mod = right ? 1 : -1;
			int nx = x + mod;

			while (nx >= 0 && nx < s.length() && std::isdigit(s[nx])) {
				if (mod > 0) {
					num.push_back(s[nx]);

				} else {
					num.insert(num.begin(), s[nx]);
				}

				nx += mod;
			}

			if (! num.empty()) {
				result = std::stoi(num);

				return true;
			}

			return false;
		};

		auto scanNum = [](const std::string &s, int x, int &result) {
			int scanX = -1;

			if (x > 0 && std::isdigit(s[x - 1])) {
				scanX = x - 1;

			} else if (std::isdigit(s[x])) {
				scanX = x;

			} else if (x < (s.length() - 1) && std::isdigit(s[x + 1])) {
				scanX = x + 1;
			}

			if (scanX >= 0){
				std::string num(1, s[scanX]);
				int pos;

				pos = scanX;
				while (--pos >= 0 && std::isdigit(s[pos])) {
					num.insert(num.begin(), s[pos]);
				}

				pos = scanX;
				while (++pos < s.length() && std::isdigit(s[pos])) {
					num.push_back(s[pos]);
				}

				result = std::stoi(num);
				return true;
			}

			return false;
		};

		auto scanNums = [&scanNum](const std::string &s, int x, std::vector<int> &result) {
			int num;

			if (std::isdigit(s[x])) {
				if (scanNum(s, x, num)) {
					result.push_back(num);
				}

			} else {
				if (std::isdigit(s[x - 1]) && scanNum(s, x - 1, num)) {
					result.push_back(num);
				}

				if (std::isdigit(s[x + 1]) && scanNum(s, x + 1, num)) {
					result.push_back(num);
				}
			}
		};

		int partA = 0;
		int partB = 0;

		for (int y = 0; y < lines.size(); y++) {
			auto &line = lines[y];

			for (int x = 0; x < line.length(); x++) {
				char c = line[x];

				if (! std::isdigit(c) && c != '.') {
					std::vector<int> parts;

					int num;

					if (parseNum(line, x, false, num)) {
						parts.push_back(num);
					}

					if (parseNum(line, x, true, num)) {
						parts.push_back(num);
					}

					if (y > 0) {
						scanNums(lines[y - 1], x, parts);
					}

					if (y < lines.size() - 1) {
						scanNums(lines[y + 1], x, parts);
					}

					DBG(("Symbol: '%c'", c));
					for (auto n : parts) {
						DBG(("   NUmber: %d", n));
					}

					if (! parts.empty()) {
						for (auto part : parts) {
							partA += part;
						}

						if (c == '*') {
							if (parts.size() == 2) {
								partB += parts[0] * parts[1];
							}
						}
					}
				}
			}
		}

		PRINTF(("PART_A: %d", partA));
		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
