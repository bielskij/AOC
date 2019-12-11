#include <stdexcept>
#include <climits>
#include <set>
#include <algorithm>

#include <openssl/md5.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::vector<std::string> data = File::readAllLines(argv[1]);

	{
		int niceCount = 0;

		for (auto str = data.begin(); str != data.end(); str++) {
			bool isNice = true;

			if (isNice) {
				int vowels = 0;

				vowels += std::count(str->begin(), str->end(), 'a');
				vowels += std::count(str->begin(), str->end(), 'e');
				vowels += std::count(str->begin(), str->end(), 'i');
				vowels += std::count(str->begin(), str->end(), 'o');
				vowels += std::count(str->begin(), str->end(), 'u');

				isNice = vowels >= 3;
			}

			if (isNice) {
				char lastChar = 0;

				isNice = false;

				for (auto c = str->begin(); c != str->end(); c++) {
					if (lastChar == *c) {
						isNice = true;
						break;
					}

					lastChar = *c;
				}
			}

			if (isNice) {
				isNice =
					str->find("ab") == std::string::npos &&
					str->find("cd") == std::string::npos &&
					str->find("pq") == std::string::npos &&
					str->find("xy") == std::string::npos;
			}

			if (isNice) {
				niceCount++;
			}
		}

		PRINTF(("PART A: %d", niceCount));
	}

	{
		int niceCount = 0;

		for (auto str = data.begin(); str != data.end(); str++) {
			bool isNice = true;

			if (isNice) {
				isNice = false;

				for (int i = 0; i < str->length() - 1; i++) {
					if (utils::countSubstring(*str, str->substr(i, 2)) >= 2) {
						isNice = true;
						break;
					}
				}
			}

			if (isNice) {
				isNice = false;

				for (int i = 0; i < str->length() - 2; i++) {
					if (str->at(i) == str->at(i + 2)) {
						isNice = true;
						break;
					}
				}
			}

			if (isNice) {
				niceCount++;
			}
		}

		PRINTF(("PART B: %d", niceCount));
	}
}
