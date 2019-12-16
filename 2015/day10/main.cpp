#include <climits>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static std::string _getLookAndSay(const std::string &src) {
	std::string ret;

	{
		char last = 0;
		char num  = 0;

		for (int i = 0; i < src.size(); i++) {
			if (last != src[i]) {
				if (num) {
					ret += (num + 48);
					ret += last;
				}

				last = src[i];
				num  = 1;

			} else {
				num++;
			}
		}

		if (num) {
			ret += (num + 48);
			ret += last;
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	std::string data = argv[1];

	for (int i = 0; i < utils::toInt(argv[2]); i++) {
		data = _getLookAndSay(data);
	}

	PRINTF(("RESULT: %zd", data.size()));
}
