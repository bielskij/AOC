#include <functional>
#include <cassert>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Monkey {
	int                             id;
	int                             divider;
	int                             trueMonkey;
	int                             falseMonkey;
	std::vector<int64_t>            items;
	std::function<int64_t(int64_t)> operation;
	int                             inspectNumber;

	Monkey() {
		this->id            = -1;
		this->divider       = 1;
		this->trueMonkey    = -1;
		this->falseMonkey   = -1;
		this->inspectNumber = 0;
	}

	void dump() {
		printf("Monkey %d\n", this->id);
		printf(" on false: %d\n", this->falseMonkey);
		printf(" on true:  %d\n", this->trueMonkey);
		printf(" divider:  %d\n", this->divider);
		printf(" items: ");

		for (auto item : this->items) {
			printf("%lld, ", (long long int) item);
		}
		printf("\n");
	}
};


static long long int solve(std::vector<Monkey> monkeys, int rounds, std::function<int64_t(int64_t)> mod) {
	for (int round = 0; round < rounds; round++) {
		for (auto &monkey : monkeys) {
			for (auto item: monkey.items) {
				int64_t newItem = mod(monkey.operation(item));

				monkey.inspectNumber++;

				if (newItem % monkey.divider == 0) {
					monkeys[monkey.trueMonkey].items.push_back(newItem);

				} else {
					monkeys[monkey.falseMonkey].items.push_back(newItem);
				}
			}

			monkey.items.clear();
		}
	}

	{
		std::vector<int> inspectSorted;

		for (auto &m : monkeys) {
			inspectSorted.push_back(m.inspectNumber);
		}

		std::sort(inspectSorted.begin(), inspectSorted.end(), std::greater());

		return (long long int) inspectSorted[0] * (long long int) inspectSorted[1];
	}
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Monkey> monkeys;

		{
			Monkey monkey;

			enum {
				PARSE_ID,
				PARSE_ITEMS,
				PARSE_OPERATION,
				PARSE_TEST,
				PARSE_TRUE,
				PARSE_FALSE
			} state = PARSE_ID;

			for (auto &line : lines) {
				if (line.empty()) {
					continue;
				}

				switch (state) {
					case PARSE_ID:
						{
							assert(sscanf(line.c_str(), "Monkey %d:", &monkey.id) == 1);

							state = PARSE_ITEMS;
						}
						break;

					case PARSE_ITEMS:
						{
							monkey.items = utils::toIntV<int64_t>(
								utils::strTok(line.substr(18), ',')
							);

							state = PARSE_OPERATION;
						}
						break;

					case PARSE_OPERATION:
						{
							auto toks = utils::strTok(line.substr(13), ' ');

							assert(toks.size() == 5);

							char op = toks[3][0];
							if (toks[4] == "old") {
								monkey.operation = [op](int64_t item) {
									if (op == '*') {
										return item * item;
									} else {
										return item + item;
									}
								};

							} else {
								int r = utils::toInt(toks[4]);

								monkey.operation = [op, r](int64_t item) {
									if (op == '*') {
										return item * r;
									} else {
										return item + r;
									}
								};
							}

							state = PARSE_TEST;
						}
						break;

					case PARSE_TEST:
						{
							assert(sscanf(utils::trim(line, " ").c_str(), "Test: divisible by %d", &monkey.divider) == 1);

							state = PARSE_TRUE;
						}
						break;

					case PARSE_TRUE:
						{
							assert(sscanf(utils::trim(line, " ").c_str(), "If true: throw to monkey %d", &monkey.trueMonkey) == 1);

							state = PARSE_FALSE;
						}
						break;

					case PARSE_FALSE:
						{
							assert(sscanf(utils::trim(line, " ").c_str(), "If false: throw to monkey %d", &monkey.falseMonkey) == 1);

							state = PARSE_ID;
						}
						break;
				}

				if (state == PARSE_ID) {
					monkeys.push_back(monkey);

					monkey = Monkey();
				}
			}

			for (auto &m : monkeys) {
				m.dump();
			}
		}

		PRINTF(("PART_A: %lld", solve(monkeys, 20, [](int64_t item) {
			return item / 3;
		})));

		{
			int commonMod = 1;

			for (auto &m : monkeys) {
				commonMod *= m.divider;
			}

			PRINTF(("PART_B: %lld", solve(monkeys, 10000, [commonMod](int64_t item) {
				return item % commonMod;
			})));
		}
	}

	return 0;
}
