#include <stdexcept>
#include <string.h>

#include "utils/file.h"
#include "utils/utils.h"

#include "common/debug.h"


static uint64_t _getDecompressedLength(const std::string &src, bool recursive) {
	uint64_t length = 0;

	std::string result = src;

	for (int c = 0; c < result.length(); c++) {
		if (result[c] != '(') {
			length++;

		} else {
			std::string marker;

			for (int last = c; last < result.length(); last++) {
				if (result[last] == ')') {
					marker = result.substr(c, last - c + 1);
					break;
				}
			}

			int chars, repeated;

			if (sscanf(marker.c_str(), "(%dx%d)", &chars, &repeated) != 2) {
				abort();
			}

			if (recursive) {
				std::string resolved;
				std::string word = result.substr(c + marker.size(), chars);

				resolved.reserve(chars * repeated);

				for (int l = 0; l < repeated; l++) {
					resolved.append(word);
				}

				length += _getDecompressedLength(resolved, true);

			} else {
				length += (chars * repeated);
			}

			c += (marker.size() + chars - 1);
		}
	}

	return length;
}


int main(int argc, char *argv[]) {
	auto line = File::readAllLines(argv[1])[0];

	PRINTF(("PART_A: %" PRIu64, _getDecompressedLength(line, false)));
	PRINTF(("PART_B: %" PRIu64, _getDecompressedLength(line, true)));
}
