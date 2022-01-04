#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int partA = 0;

		for (const auto &l : lines) {
			auto words = utils::strTok(l, ' ');

			std::set<std::string> uniqueWords(words.begin(), words.end());

			if (uniqueWords.size() == words.size()) {
				partA++;
			}
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		int partB = 0;

		for (const auto &l : lines) {
			auto words = utils::strTok(l, ' ');

			std::set<std::string> uniqueWords;

			for (const auto &w : words) {
				std::string tmp = w;

				std::sort(tmp.begin(), tmp.end());

				uniqueWords.insert(tmp);
			}


			if (uniqueWords.size() == words.size()) {
				partB++;
			}
		}

		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
