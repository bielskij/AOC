#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

struct Sector {
	bool freeSpace;
	int  id;

	Sector(int id, bool isFreeSpace) {
		this->freeSpace = isFreeSpace;
		this->id        = this->freeSpace ? -1 : id;
	}
};

struct Block {
	int freeSpace;
	int id;
	int length;

	Block(int id, int length, int freeSpace) {
		this->freeSpace = freeSpace;
		this->id        = id;
		this->length    = length;
	}
};

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Sector> sectors;
		std::vector<Block> blocks;

		{
			bool isFree = false;
			int  id = 0;

			for (auto c : lines[0]) {
				auto length = c - '0';

				for (int i = 0; i < length; i++) {
					sectors.push_back(Sector(id, isFree));
				}
				
				if (isFree) {
					id++;
					blocks.rbegin()->freeSpace = length;

				} else {
					blocks.push_back(Block(id, length, 0));
				}

				isFree = ! isFree;
			}
		}

		{
			auto toMoveIt = sectors.end() - 1;
			auto emptyIt  = sectors.begin();

			while (emptyIt < toMoveIt) {
				while (toMoveIt->freeSpace && toMoveIt != sectors.begin()) {
					--toMoveIt;
				}

				if (toMoveIt != sectors.begin()) {
					while (! emptyIt->freeSpace && emptyIt != sectors.end()) {
						++emptyIt;
					}

					if (emptyIt < toMoveIt) {
						std::swap(*toMoveIt, *emptyIt);
					}
				}
			}

			int64_t partA = 0;

			for (int i = 0; i < sectors.size(); i++) {
				auto &b = sectors[i];

				if (! b.freeSpace) {
					partA += (i * b.id);
				}
			}

			PRINTF(("PART_A: %ld", partA));
		}

		{
			auto toMove = blocks.rbegin();

			while (toMove != blocks.rend()) {
				auto freeNode = blocks.begin();
				
				while (freeNode < toMove.base() - 1) {
					if (freeNode->freeSpace >= toMove->length) {
						auto dist = std::distance(freeNode, toMove.base()) - 1;

						if (toMove != blocks.rend()) {
							(toMove + 1)->freeSpace += (toMove->freeSpace + toMove->length);
						}

						toMove->freeSpace = freeNode->freeSpace - toMove->length;
						freeNode->freeSpace = 0;

						toMove = std::make_reverse_iterator(blocks.erase(blocks.insert(freeNode + 1, *toMove) + dist));
						break;
					}
					++freeNode;
				}

				if (freeNode >= toMove.base() - 1) {
					++toMove;
				}
			}

			{
				int64_t partB = 0;
				int64_t position = 0;
				for (auto &b : blocks) {
					for (int i = 0; i < b.length; i++) {
						partB += (int64_t)(b.id * position++);
					}

					position += b.freeSpace;
				}

				PRINTF(("PART_B: %ld", partB));
			}
		}
	}

	return 0;
}
