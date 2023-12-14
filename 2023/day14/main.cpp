#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


enum class Mode {
	NORTH,
	WEST,
	SOUTH,
	EAST
};


static void tilt(std::vector<std::string> &platform, Mode mode) {
	switch (mode) {
		case Mode::NORTH:
			for (int row = 1; row < platform.size(); row++) {
				auto &l = platform[row];

				for (int col = 0; col < l.length(); col++) {
					char c = l[col];

					if (c == 'O') {
						int depth = row - 1;

						for (; depth >= 0; depth--) {
							if (platform[depth][col] == '.') {
								platform[depth + 1][col] = '.';
								platform[depth    ][col] = 'O';

							} else {
								break;
							}
						}
					}
				}
			}
			break;

		case Mode::SOUTH:
			for (int row = platform.size() - 2; row >= 0; row--) {
				auto &l = platform[row];

				for (int col = 0; col < l.length(); col++) {
					char c = l[col];

					if (c == 'O') {
						int depth = row + 1;

						for (; depth < platform.size(); depth++) {
							if (platform[depth][col] == '.') {
								platform[depth - 1][col] = '.';
								platform[depth    ][col] = 'O';

							} else {
								break;
							}
						}
					}
				}
			}
			break;

		case Mode::EAST:
			for (int col = platform[0].length() - 2; col >= 0 ; col--) {
				for (int row = 0; row < platform.size(); row++) {
					auto &l = platform[row];

					if (l[col] == 'O') {
						int depth = col + 1;

						for (; depth < platform[0].length(); depth++) {
							if (l[depth] == '.') {
								l[depth - 1] = '.';
								l[depth    ] = 'O';

							} else {
								break;
							}
						}
					}
				}
			}
			break;

		case Mode::WEST:
			for (int col = 1; col < platform[0].length() ; col++) {
				for (int row = 0; row < platform.size(); row++) {
					auto &l = platform[row];

					if (l[col] == 'O') {
						int depth = col - 1;

						for (; depth >= 0; depth--) {
							if (l[depth] == '.') {
								l[depth + 1] = '.';
								l[depth    ] = 'O';

							} else {
								break;
							}
						}
					}
				}
			}
			break;
	}
}


static void cycle(std::vector<std::string> &platform, int cycles) {
	std::set<std::string> cache;

	int lastPos = 0;

	for (int i = 0; i < cycles; i++) {
		tilt(platform, Mode::NORTH);
		tilt(platform, Mode::WEST);
		tilt(platform, Mode::SOUTH);
		tilt(platform, Mode::EAST);
	}
}


static std::pair<int, int> findCycleOffsetAndLength(const std::vector<std::string> &platform) {
	auto p = platform;

	std::set<std::string> cache;

	int offset = -1;
	int length = -1;

	for (int i = 0; true; i++) {
		cycle(p, 1);

		{
			std::string key;

			key.reserve(p.size() * p[0].length());

			for (auto &r : p) {
				key += r;
			}

			if (! cache.insert(key).second) {
				if (offset < 0) {
					cache.clear();

					offset = i;

				} else {
					length = cache.size();
					break;
				}
			}
		}
	}

	return { offset, length };
}


static int calcLoad(const std::vector<std::string> &platform) {
	int ret = 0;

	for (int col = 0; col < platform[0].size(); col++) {
		int load = 0;

		for (int row = 0; row < platform.size(); row++) {
			char c = platform[row][col];

			if (c == 'O') {
				load += platform.size() - row;
			}
		}

		ret += load;
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		auto platform = lines;

		tilt(platform, Mode::NORTH);

		PRINTF(("PART_A: %d", calcLoad(platform)));
	}

	{
		auto platform = lines;

		auto p = findCycleOffsetAndLength(platform);

		cycle(platform, p.first);
		cycle(platform, (1000000000 - p.first) % p.second);

		PRINTF(("PART_B: %d", calcLoad(platform)));
	}

	return 0;
}
