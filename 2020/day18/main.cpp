#include <stack>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

/*
 * Based on https://www.geeksforgeeks.org/expression-evaluation/
 */
int64_t applyOp(int64_t a, int64_t b, char op){
	DBG(("%s(%" PRId64 " %c %" PRId64 ")", __func__, a, op, b));

	switch(op){
		case '+': return a + b;
		case '*': return a * b;
	}

	return 0;
}


static void executeTop(std::stack <int64_t>  &values, std::stack <char> &ops) {
	int64_t valueRight = values.top(); values.pop();
	int64_t valueLeft  = values.top(); values.pop();
	char    operator_  = ops.top();    ops.pop();

	values.push(applyOp(valueLeft, valueRight, operator_));
}


static int64_t eval(const std::string &str, int (*precedence)(char)) {
	int i;

	std::stack <int64_t>  values;
	std::stack <char> ops;

	for (i = 0; i < str.length(); i++) {
		char letter = str[i];

		if (letter == ' ') {
			continue;

		} else if (letter == '(') {
			ops.push(letter);

		} else if (isdigit(letter)){
			int64_t val = 0;

			while(i < str.length() && isdigit(str[i])) {
				val = (val * 10) + (str[i] - '0');
				i++;
			}

			values.push(val);

			i--;

		} else if (letter == ')') {
			while (! ops.empty() && ops.top() != '(') {
				executeTop(values, ops);
			}

			if (! ops.empty()) {
				ops.pop();
			}

		} else {
			while (! ops.empty() && (precedence(ops.top()) >= precedence(letter))){
				executeTop(values, ops);
			}

			ops.push(letter);
		}
	}

	while (! ops.empty()) {
		executeTop(values, ops);
	}

	return values.top();
}


int precedencePartA(char op) {
	if (op == '+' || op == '*') {
		return 1;
	}

	return 0;
}


int precedencePartB(char op) {
	if (op == '+') {
		return 2;

	} else if (op == '*') {
		return 1;
	}

	return 0;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int64_t sum = 0;
		for (auto &line : lines) {
			sum += eval(line, precedencePartA);
		}
		PRINTF(("PART_A: %" PRId64, sum));
	}

	{
		int64_t sum = 0;
		for (auto &line : lines) {
			sum += eval(line, precedencePartB);
		}
		PRINTF(("PART_B: %" PRId64, sum));
	}

	return 0;
}
