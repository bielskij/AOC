#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <unistd.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class SpringDroid : public IntCodeMachine {
	public:
		SpringDroid(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			this->springScriptOffset = 0;
			this->output = 0;
		}

		bool onOut(int64_t value) {
//			printf("%c", (char) value);

			if (value > 255) {
				this->output = value;
			}

			return true;
		}

		bool onIn(int64_t &value) {
			if (this->springScriptOffset < this->springScript.length()) {
				value = this->springScript[this-> springScriptOffset];

				this->springScriptOffset++;

				return true;
			}

			return false;
		}

		/*
		 * jump 4 tiles
		 * A, B, C, D - sensors (1, 2, 3, 4 tiles)
		 * T - temp
		 * J - JUMP
		 * AND, OR, NOT - max 15 instructions
		 *
		 * https://www.charlie-coleman.com/experiments/kmap/
		 *
		 *  @
		 * #####.###########
		 * #####...#########
		 * #####..#.########
		 *
		 *  #####.###########
		 *   1111       0 *
		 *    1110    - 0 *
		 *     1101   - 1 *
		 *      1011  - 1 *
		 *       0111 - 1 *
		 *
		 *  #####...####### ##
		 *   1111       0 *
		 *    1110    - 0 *
		 *     1100   - 0 *
		 *      1000  - 0 *
		 *       0001 - 1 *
		 *
		 *  #####..#.########
		 *   1111       0 *
		 *    1110      0 *
		 *     1100   - 0 *
		 *      1001  - 1 *
		 *       0010 - 0 *
		 *
		 *   (~c & d) | (~a & d) | (a & ~b & c)
		 */
		void partA() {
			this->springScriptOffset = 0;
			this->springScript =
				"NOT C T\n"
				"AND D T\n"
				"NOT A J\n"
				"AND D J\n"
				"OR  J T\n"
				"NOT B J\n"
				"AND A J\n"
				"AND C J\n"
				"OR T J\n"
				"WALK\n";

			this->reset();
			this->run();

			PRINTF(("PART_A: %d", this->output));
		}

		/*
		 * #####.#.##...####
		 * 111110101         0 501
		 *  111101011        0 491
		 *   111010110       0 470
		 *    110101100      0 428
		 *     101011000     1 344
		 *      010110001    0 177
		 *       101100011   0 355
		 *        011000111  0 199
		 *         110001111 0 399
		 * #####..#.########
		 * 111110010         0 498
		 *  111100101        0 485
		 *   111001011       0 459
		 *    110010111      0 407
		 *     100101111     1 303
		 *      001011111    0  95
		 *       010111111   0 191
		 *        101111111  0 383
		 *         011111111 1 255
		 * #####...#########
		 * 111110001         0 497
		 *  111100011        0 483
		 *   111000111       0 455
		 *    110001111      0 399
		 *     100011111     0 287
		 *      000111111    1  63
		 *       001111111   0 127
		 *        011111111  0 255
		 *         111111111 0 511
		 * #####.###########
		 * 111110111         0 503
		 *  111101111        0 495
		 *   111011111       0 479
		 *    110111111      0 447
		 *     101111111     0 383
		 *      011111111    1 255
		 *       111111111   0 511
		 * #####.#.#########
		 * 111110101         0 501
		 *  111101011        0 491
		 *   111010111       0 471
		 *    110101111      0 431
		 *     101011111     0 351
		 *      010111111    1 191
		 *       101111111   0 383
		 *        011111111  0 255
		 *
		 * 101011000
		 * 100101111
		 * 011111111
		 * 000111111
		 * 010111111
		 * 011111111
		 */
		void partB() {
			this->springScriptOffset = 0;
			this->springScript =
				"NOT A J\n"

				"NOT C T\n"
				"AND H T\n"
				"OR T J\n"

				"NOT B T\n"
				"AND A T\n"
				"AND C T\n"
				"OR T J\n"

				"AND D J\n"
				"RUN\n";

			this->reset();
			this->run();

			PRINTF(("PART_B: %d", this->output));
		}

	private:
		std::string springScript;
		int         springScriptOffset;
		int         output;
};

int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		SpringDroid droid(program);

		droid.partA();
		droid.partB();
	}
}
