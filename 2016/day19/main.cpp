#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Elf {
	int presents;
	int idx;

	Elf *left;
	Elf *right;

	Elf() {
		this->presents = 0;
		this->idx      = 0;
		this->left     = nullptr;
		this->right    = nullptr;
	}
};


static void initElfs(Elf *elfs, int elfCount) {
	for (int i = 0; i < elfCount; i++) {
		if (i == elfCount - 1) {
			elfs[i].left = &elfs[0];

		} else {
			elfs[i].left = &elfs[i + 1];
		}

		if (i == 0) {
			elfs[i].right = &elfs[elfCount - 1];

		} else {
			elfs[i].right = &elfs[i - 1];
		}

		elfs[i].presents = 1;
		elfs[i].idx      = i + 1;
	}
}


int partA(int elfCount) {
	int ret = 0;

	{
		Elf *elfs = new Elf[elfCount];

		initElfs(elfs, elfCount);

		Elf *e = elfs;
		do {
			if (e->presents == 0) {
				Elf *prev = e->right;
				Elf *next = e->left;

				// remove elf
				prev->left  = next;
				next->right = prev;

			} else {
				if (e->left->presents > 0) {
					e->presents += e->left->presents;

					e->left->presents = 0;
				}
			}

			if (e->presents != elfCount) {
				e = e->left;
			}
		} while (e->presents != elfCount);

		ret = e->idx;

		delete elfs;
	}

	return ret;
}


int partB(int elfCount) {
	int ret = elfCount;

	{
		Elf *elfs = new Elf[elfCount];

		initElfs(elfs, elfCount);

		Elf *e = elfs;
		do {
			Elf *acrossElf = e;
			int offset = elfCount / 2;

			while (offset > 0) {
				acrossElf = acrossElf->left;
				offset--;
			}
PRINTF(("E: %d, across: %d", e->idx, acrossElf->idx));
			e->presents += acrossElf->presents;

			// remove elf
			{
				Elf *prev = acrossElf->right;
				Elf *next = acrossElf->left;

				prev->left  = next;
				next->right = prev;
			}

			elfCount--;

			if (e->presents != ret) {
				e = e->left;
			}
		} while (e->presents != ret);

		ret = e->idx;

		delete elfs;
	}

	return ret;
}


int main(int argc, char *argv[]) {
	int elfCount = utils::toInt(argv[1]);

	PRINTF(("PART_A: %d", partA(elfCount)));
	PRINTF(("PART_B: %d", partB(elfCount)));
}
