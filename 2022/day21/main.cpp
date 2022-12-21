#include <cassert>
#include <cstring>
#include <functional>
#include <boost/process.hpp>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Monkey {
	virtual ~Monkey() {

	}

	virtual int64_t getNumber(std::map<std::string, Monkey *> &monkeys) = 0;
	virtual std::string getEquation(std::map<std::string, Monkey *> &monkeys) = 0;
};


struct MonkeyNumber : public Monkey {
	MonkeyNumber(int64_t num) {
		this->num     = num;
		this->handler = [this]() {
			return std::to_string(this->num);
		};
	}

	int64_t getNumber(std::map<std::string, Monkey *> &monkeys) override {
		return this->num;
	}

	std::string getEquation(std::map<std::string, Monkey *> &monkeys) override {
		return handler();
	}

	std::function<std::string()> handler;

	int64_t num;
};


struct MonkeyOperation : public Monkey {
	MonkeyOperation(const std::string &lId, const std::string &rId, char op) : lId(lId), rId(rId) {
		this->op = op;

		this->handler = [](std::map<std::string, Monkey *> &monkeys, const std::string &lId, const std::string &rId, char op) {
			int64_t ret = -1;

			switch (op) {
				case '+': ret = monkeys[lId]->getNumber(monkeys)  + monkeys[rId]->getNumber(monkeys); break;
				case '-': ret = monkeys[lId]->getNumber(monkeys)  - monkeys[rId]->getNumber(monkeys); break;
				case '*': ret = monkeys[lId]->getNumber(monkeys)  * monkeys[rId]->getNumber(monkeys); break;
				case '/': ret = monkeys[lId]->getNumber(monkeys)  / monkeys[rId]->getNumber(monkeys); break;
				default:
					assert(false);
			}

			return ret;
		};
	}

	int64_t getNumber(std::map<std::string, Monkey *> &monkeys) override {
		return this->handler(monkeys, lId, rId, op);
	}

	std::string getEquation(std::map<std::string, Monkey *> &monkeys) {
		return "(" + monkeys[lId]->getEquation(monkeys) + " " + op + " " + monkeys[rId]->getEquation(monkeys) + ")";
	}

	char op;
	std::string lId;
	std::string rId;

	std::function<int64_t(std::map<std::string, Monkey *> &monkeys, const std::string &lId, const std::string &rId, char op)> handler;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::map<std::string, Monkey *> monkeys;

	for (auto &l : lines) {
		auto toks = utils::strTok(l, ':');

		assert(toks.size() == 2);

		{
			auto opToks = utils::strTok(utils::trim(toks[1], " "), ' ');

			if (opToks.size() == 1) {
				monkeys[toks[0]] = new MonkeyNumber(utils::toInt(opToks[0]));

			} else {
				assert(opToks.size() == 3);

				monkeys[toks[0]] = new MonkeyOperation(opToks[0], opToks[2], opToks[1][0]);
			}
		}
	}

	PRINTF(("PART_A: %lld", monkeys["root"]->getNumber(monkeys)));

	{
		MonkeyOperation *root = dynamic_cast<MonkeyOperation *>(monkeys["root"]);
		MonkeyNumber    *humn = dynamic_cast<MonkeyNumber *>(monkeys["humn"]);

		assert(root);
		assert(humn);

		humn->handler = []() {
			return "x";
		};

		root->op = '=';

		root->handler = [](std::map<std::string, Monkey *> &monkeys, const std::string &lId, const std::string &rId, char op) {
			return (int64_t) (monkeys[lId]->getNumber(monkeys) == monkeys[rId]->getNumber(monkeys));
		};

		std::string equation = root->getEquation(monkeys);

		equation = equation.substr(1, equation.size() - 2);

		int64_t xValue = 0;

		{
			auto cmd = "bash -c \"mathomatic -e \\\"" + equation + "\\\" -e x -q -c | grep 'x = ' | awk '{print \\$NF}'\"";

			FILE *pipe = popen(cmd.c_str(), "r");
			if (pipe) {
				while (! feof(pipe)) {
					char buffer[128];

					if (fgets(buffer, sizeof(buffer) - 1, pipe)) {
						xValue = strtoll(buffer, nullptr, 10);
						break;
					}
				}

				fclose(pipe);
			}
		}

		humn->num = xValue;

		assert(root->getNumber(monkeys) == 1);

		PRINTF(("PART_B: %lld", xValue));
	}

	return 0;
}
