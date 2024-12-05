#include <regex>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int64_t _execute(const std::vector<std::string> &lines, bool partB) {
	int64_t ret = 0;

	std::regex reg(
		partB ?
		"(mul)\\(([0-9]{1,3}),([0-9]{1,3})\\)|(do)\\(\\)|(don't)\\(\\)" :
		"(mul)\\(([0-9]{1,3}),([0-9]{1,3})\\)"
	);

	bool doMul = true;

	for (const auto &l : lines) {
		auto words_begin = std::sregex_iterator(l.begin(), l.end(), reg);
		auto words_end   = std::sregex_iterator();

		for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
			std::smatch match = *i;

			if (! match[1].str().empty()) {
				if (doMul) {
					ret += (std::stold(match[2].str()) * std::stold(match[3].str()));
				}
			} else if (! match[4].str().empty()) {
				doMul = true;

			} else if (! match[5].str().empty()) {
				doMul = false;
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);
	
	PRINTF(("PART_A: %ld", _execute(lines, false)));
	PRINTF(("PART_B: %ld", _execute(lines, true)));

	return 0;
}
