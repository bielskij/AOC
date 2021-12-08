#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static uint8_t charDigitToSegmentBits(const std::string &strDigit) {
	uint8_t ret = 0;

	for (auto c : strDigit) {
		ret |= (1 << (c - 'a'));
	}

	return ret;
}


static uint8_t bitsCount(uint8_t val) {
	uint8_t ret = 0;

	for (uint8_t i = 0; i < 8; i++) {
		if ((val & (1 << i)) != 0) {
			ret++;
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int partA = 0;
		int partB = 0;

		for (const auto &e : lines) {
			auto pair = utils::strTok(e, '|');

			std::string uniqueSignalPatterns = utils::trim(pair[0], " ");
			std::string outputValue          = utils::trim(pair[1], " ");

			for (const auto &signal : utils::strTok(outputValue, ' ')) {
				if (
					(signal.length() == 2) ||
					(signal.length() == 4) ||
					(signal.length() == 3) ||
					(signal.length() == 7)
				) {
					partA++;
				}
			}

			/*      0        0 - 6
			 *   aaaaaaa     1 - 2 *
			 *   b     d     2 - 5
			 * 1 b     d 2   3 - 5
			 *   b  3  d     4 - 4 *
			 *   ccccccc     5 - 5
			 *   e     g     6 - 6
			 * 4 e     g 5   7 - 3 *
			 *   e  6  g     8 - 7 *
			 *   fffffff     9 - 6
			 */

			// Maps segments (0-7) to bit flags
			uint8_t segmentToValMap[7] = { 0 };
			// Maps display digit to segment bit map
			uint8_t digits[10]         = { 0 };
			// maps digit segment bitmaps to real value
			std::map<uint8_t, uint8_t> digitsToVal;

			{
				std::map<int, std::vector<std::string>> samples;

				auto signals = utils::strTok(uniqueSignalPatterns, ' ');
				for (const auto &signal : signals) {
					switch (signal.length()) {
						case 2: digits[1] = charDigitToSegmentBits(signal); break;
						case 3: digits[7] = charDigitToSegmentBits(signal); break;
						case 4: digits[4] = charDigitToSegmentBits(signal); break;
						case 7: digits[8] = charDigitToSegmentBits(signal); break;
						default:
							samples[signal.length()].push_back(signal);
							break;
					}
				}

				// Segment 0
				segmentToValMap[0] = digits[1] ^ digits[7];

				// Segments 2, 5 and 6, digits of 6 segments: 9, 6, 0
				for (const auto sample : samples[6]) {
					uint8_t bitMask = charDigitToSegmentBits(sample);

					// Segments 2 and 5
					if (bitsCount(bitMask & digits[1]) == 1) {
						digits[6] = bitMask;

						segmentToValMap[5] = bitMask & digits[1];
						segmentToValMap[2] = segmentToValMap[5] ^ digits[1];
					}

					// segment 6
					if (bitsCount(bitMask ^ (digits[4] | digits[7])) == 1) {
						digits[9] = bitMask;

						segmentToValMap[6] = bitMask ^ (digits[4] | digits[7]);
					}
				}

				// segment 3
				for (const auto sample : samples[6]) {
					uint8_t bitMask = charDigitToSegmentBits(sample);
					uint8_t toCheck = (bitMask ^ digits[8]) & digits[6] & digits[9];

					if (bitsCount(toCheck) == 1) {
						digits[0] = bitMask;

						segmentToValMap[3] = bitMask ^ digits[8];
					}
				}

				// segment 4
				segmentToValMap[4] = digits[9] ^ digits[8];

				digits[3] = segmentToValMap[0] | segmentToValMap[2] | segmentToValMap[3] | segmentToValMap[5] | segmentToValMap[6];

				// segment 1
				segmentToValMap[1] = digits[3] ^ digits[9];

				digits[2] = segmentToValMap[0] | segmentToValMap[2] | segmentToValMap[3] | segmentToValMap[4] | segmentToValMap[6];
				digits[5] = segmentToValMap[0] | segmentToValMap[1] | segmentToValMap[3] | segmentToValMap[5] | segmentToValMap[6];

				for (int i = 0; i < 10; i++) {
					digitsToVal[digits[i]] = i;
				}
#if 0
				PRINTF((" "));
				for (int i = 0; i < 7; i++) {
					PRINTF(("segment[%d] = %02x (%d)", i, segmentToValMap[i], segmentToValMap[i]));
				}

				PRINTF((" "));
				for (int i = 0; i < 10; i++) {
					PRINTF(("digit[%d] = %02x (%d)", i, digits[i], digits[i]));
				}
#endif
			}

			{
				auto outputValues = utils::strTok(outputValue, ' ');

				int val = 0;


				for (int i = 0; i < outputValues.size(); i++) {
					int multiplier = 1;

					for (int j = 0; j < outputValues.size() - i - 1; j++) {
						multiplier *= 10;
					}

					val += (
						digitsToVal[charDigitToSegmentBits(outputValues[i])] * multiplier
					);
				}

				partB += val;
			}
		}

		PRINTF(("PART_A: %d", partA));
		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
