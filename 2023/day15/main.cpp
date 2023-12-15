#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int hashStep(int start, char c) {
	start += c;
	start *= 17;
	start %= 256;

	return start;
}


static int hash(const std::string &str) {
	int ret = 0;

	for (char c : str) {
		ret = hashStep(ret, c);
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::array<std::vector<std::pair<std::string, int>>, 256> sboxes;

		int partA = 0;

		for (auto &l : lines) {
			auto sequences = utils::strTok(l, ',');

			for (auto &seq : sequences) {
				std::string label;
				std::string focalLength;

				partA += hash(seq);

				if (*seq.rbegin() == '-') {
					label = seq.substr(0, seq.length() - 1);

				} else {
					label       = seq.substr(0, seq.find_first_of('='));
					focalLength = seq.substr(label.length() + 1);
				}

				{
					auto &sbox = sboxes[hash(label)];

					if (focalLength.empty()) {
						if (! sbox.empty()) {
							sbox.erase(std::remove_if(sbox.begin(), sbox.end(), [&label](std::pair<std::string, int> &v) {
								return v.first == label;
							}), sbox.end());
						}

					} else {
						auto it = std::find_if(sbox.begin(), sbox.end(), [&label](std::pair<std::string, int> &v) {
							return v.first == label;
						});

						if (it == sbox.end()) {
							sbox.push_back({ label, std::stoi(focalLength) });

						} else {
							it->second = std::stoi(focalLength);
						}
					}
				}
			}
		}

		PRINTF(("PART_A: %d", partA));

		{
			int partB = 0;

			for (int sboxIndex = 0; sboxIndex < sboxes.size(); sboxIndex++) {
				auto &sbox = sboxes[sboxIndex];

				for (size_t slotIndex = 0; slotIndex < sbox.size(); slotIndex++) {
					partB += ((sboxIndex + 1) * (slotIndex + 1) * sbox[slotIndex].second);
				}
			}

			PRINTF(("PART_B: %d", partB));
		}
	}

	return 0;
}
