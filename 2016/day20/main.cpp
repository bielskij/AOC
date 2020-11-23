#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct IpRange {
	uint32_t start;
	uint32_t end;

	IpRange() {
		this->start = 0;
		this->end   = UINT32_MAX;
	}

	IpRange(uint32_t start, uint32_t end) {
		this->start = start;
		this->end   = end;
	}
};


int main(int argc, char *argv[]) {
	auto rules = File::readAllLines(argv[1]);

	{
		std::list<IpRange> freeAddresses;

		freeAddresses.push_back(IpRange());

		for (auto &rule : rules) {
			uint32_t start, end;

			if (sscanf(rule.c_str(), "%" SCNu32 "-%" SCNu32, &start, &end) != 2) {
				ERR(("Scanf failed!"));
				abort();
			}

			for (auto it = freeAddresses.begin(); it != freeAddresses.end();) {

				 /*    FFFFFF
				 *    BBBBBBBB
				 *    BBBBBBB
				 *     BBBBBBB
				 *     BBBBBB
				 */
				if (start <= it->start && end >= it->end) {
					it = freeAddresses.erase(it);

				/*   FFFFF
				 *  BBBBB
				 *   B
				 *  BB
				 */
				} else if (start <= it->start && end >= it->start && end < it->end) {
					it->start = end + 1;

					if (it->start > it->end) {
						it = freeAddresses.erase(it);

					} else {
						it++;
					}

				/* FFFFF
				 *  BBBBB
				 *     B
				 *     BB
				 */
				} else if (start > it->start && start <= it->end && end >= it->end) {
					it->end = start - 1;

					if (it->start > it->end) {
						it = freeAddresses.erase(it);

					} else {
						it++;
					}

				/* FFFFFFF
				 *   BBB
				 */
				} else if (start > it->start && start < it->end && end < it->end) {
					auto nextIt = freeAddresses.insert(std::next(it), IpRange(end + 1, it->end));

					it->end = start - 1;

					it = nextIt;

				} else if (start == it->start && end == it->end) {
					it = freeAddresses.erase(it);

				} else {
					it++;
				}
			}
		}

		PRINTF(("PART_A: %u", freeAddresses.begin()->start));

		{
			uint32_t freeAddressCount = 0;

			for (auto &range : freeAddresses) {
				freeAddressCount += (range.end - range.start + 1);
			}

			PRINTF(("PART_B: %u", freeAddressCount));
		}
	}
}
