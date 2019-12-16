#include <cassert>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static bool isCorrect(const std::string &passwd) {
	bool ret = true;

	if (ret) {
		ret = false;

		for (int i = 0; i < passwd.size() - 2; i++) {
			if (
				(passwd[i + 1] == (passwd[i + 0] + 1)) &&
				(passwd[i + 2] == (passwd[i + 1] + 1))
			) {
				ret = true;
				break;
			}
		}
	}

	if (ret) {
		if (
			(passwd.find('i') != std::string::npos) ||
			(passwd.find('o') != std::string::npos) ||
			(passwd.find('l') != std::string::npos)
		) {
			ret = false;
		}
	}

	if (ret) {
		char otherGroup = 0;

		ret = false;

		for (int i = 0; i < passwd.size() - 1; i++) {
			if (passwd[i] == passwd[i + 1]) {
				if (otherGroup != 0 && otherGroup != passwd[i]) {
					ret = true;

				} else {
					otherGroup = passwd[i];
				}
			}
		}
	}

	return ret;
}


static std::string getNextPassword(const std::string &src) {
	std::string ret = src;

	do {
		*ret.rbegin() += 1;

		for (int i = ret.length() - 1; i > 0; i--) {
			if (ret[i] > 'z') {
				ret[i] = 'a';
				ret[i - 1]++;
			}
		}
	} while (! isCorrect(ret));

	return ret;
}


int main(int argc, char *argv[]) {
	std::string data = argv[1];

	assert(! isCorrect("hijklmmn"));
	assert(! isCorrect("abbceffg"));
	assert(! isCorrect("abbcegjk"));
	assert(  isCorrect("abcdffaa"));
	assert(  isCorrect("ghjaabcc"));

	data = getNextPassword(data);
	PRINTF(("PART_A: %s", data.c_str()));

	data = getNextPassword(data);
	PRINTF(("PART_B: %s", data.c_str()));
}
