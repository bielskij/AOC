#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::set<long long int> partBIds;
	std::set<long long int> partAIds;

	for (auto &line : lines) {
		auto ranges = utils::strTok(line, ',');

		for (auto &range : ranges) {
			auto limit = utils::strTok(range, '-');

			long long int start = std::stoll(limit[0]);
			long long int end   = std::stoll(limit[1]);

			for (long long int i = start; i <= end; i++) {
				std::string str = std::to_string(i);

				// Part A
				if (str.length() % 2 == 0) {
					auto sub1 = str.substr(0, str.length() / 2);
					auto sub2 = str.substr(str.length() / 2);

					if (sub1 == sub2) {
						partAIds.insert(std::stoll(str));
					}
				}

				// Part B
				for (int len = str.length() / 2; len >= 1; len--) {
					if (str.length() % len == 0) {
						bool allMatch = true;
						std::string pattern = str.substr(0, len);

						for (int off = len; off < str.length(); off += len) {
							if (str.compare(off, len, pattern) != 0) {
								allMatch = false;
								break;
							}
						}

						if (allMatch) {
							partBIds.insert(std::stoll(str));
						}
					}
				}
			}
		}
	}

	{
		long long int partA = 0;

		for (auto &id : partAIds) {
			partA += id;
		}

		ERR(("PART_A: %lld", partA));
	}

	{
		long long int partB = 0;

		for (auto &id : partBIds) {
			partB += id;
		}

		ERR(("PART_B: %lld", partB));
	}

	return 0;
}
