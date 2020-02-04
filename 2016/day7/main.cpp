#include <stdexcept>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


bool isAbba(const std::string &str) {
	if (str.size() >= 4) {
		for (int i = 0; i < str.size() - 3; i++) {
			if (
				(str[i] == str[i + 3]) && (str[i + 1] == str[i + 2]) && (str[i] != str[i + 1])
			) {
				return true;
			}
		}
	}

	return false;
}


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	int partA = 0;
	int partB = 0;

	for (auto &l : data) {
		std::vector<std::string> hypernets;
		std::vector<std::string> supernets;

		std::string word;

		bool inHyperNet = false;

		for (int i = 0; i < l.length(); i++) {
			char c = l[i];

			if (c == '[') {
				supernets.push_back(word);
				word.clear();

				inHyperNet = true;

			} else if (c == ']') {
				hypernets.push_back(word);
				word.clear();

				inHyperNet = false;

			} else {
				word += c;
			}
		}

		if (! word.empty()) {
			if (inHyperNet) {
				hypernets.push_back(word);

			} else {
				supernets.push_back(word);
			}
		}

		// PartA
		{
			bool abbaInSupernet = false;
			bool abbaInHypernet = false;

			for (auto &s : supernets) {
				if (isAbba(s)) {
					abbaInSupernet = true;
					break;
				}
			}

			for (auto &s : hypernets) {
				if (isAbba(s)) {
					abbaInHypernet = true;
					break;
				}
			}

			if (! abbaInHypernet && abbaInSupernet) {
				partA++;
			}
		}

		// PartB
		{
			std::vector<std::string> aba;

			for (auto &s : supernets) {
				for (int i = 0; i < s.length() - 2; i++) {
					if (s[i] == s[i + 2] && s[i] != s[i + 1]) {
						aba.push_back(s.substr(i, 3));
					}
				}
			}

			if (! aba.empty()) {
				bool hasAba = false;

				for (auto &a : aba) {
					std::string bab;

					bab.push_back(a[1]);
					bab.push_back(a[0]);
					bab.push_back(a[1]);

					for (auto &h : hypernets) {
						if (h.find(bab) != std::string::npos) {
							hasAba = true;
							break;
						}
					}

					if (hasAba) {
						break;
					}
				}

				if (hasAba) {
					partB++;
				}
			}
		}
	}

	PRINTF(("PART_A: %d", partA));
	PRINTF(("PART_B: %d", partB));
}
