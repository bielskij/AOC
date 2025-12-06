#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

struct Problem {
    std::vector<std::string> factors;
    char operation;
};

static std::string _normalizeSpaces(const std::string& s) {
    std::string out;
    
    out.reserve(s.size());

    bool inSpace = false;

    for (char c : s) {
        if (std::isspace((unsigned char)c)) {
            if (! inSpace) {
                inSpace = true;
                out    += c;
            }

        } else {
            inSpace = false;
            out    += c;
        }
    }

    return out;
}

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

    {
        std::vector<Problem> problems;

        for (int i = 0; i < lines.size(); i++) {
            auto toks = utils::strTok(utils::trim(_normalizeSpaces(lines[i]), " "), ' ');

            if (problems.empty()) {
                problems.resize(toks.size());
            }

            for (int j = 0; j < toks.size(); j++) {
                if (i == lines.size() - 1) {
                    problems[j].operation = toks[j][0];

                } else {
                    problems[j].factors.push_back(toks[j]);
                }
            }
        }

        {
            int64_t partA = 0;

            for (auto &problem : problems) {
                int64_t result = 0;

                switch(problem.operation) {
                    case '+': 
                        for (auto f : problem.factors) {
                            result += std::stoll(f);
                        }
                        break;

                    case '*':
                        result = 1;
                        for (auto f : problem.factors) {
                            result *= std::stoll(f);
                        }
                        break;
                }
                partA += result;
            }

            PRINTF(("PART_A: %lld", (long long int) partA));
        }
    }

    {
        std::vector<Problem> problems;

        {
            int off = 0;

            std::vector<std::string> factors;
            char operation;

            do {
                std::string factor;

                {
                    char c = lines.rbegin()->at(off);
                    if (c != ' ') {
                        operation = c;
                    }
                }

                for (int row = 0; row < lines.size() - 1; row++) {
                    auto c = lines[row][off];

                    if (c != ' ') {
                        factor.push_back(c);
                    }
                }

                if (factor.empty()) {
                    problems.push_back({
                        factors, operation
                    });

                    factors.clear();
                    operation = 0;

                } else {
                    factors.push_back(factor);
                }
            } while (++off < lines[0].length());

            if (operation != 0) {
                problems.push_back({
                    factors, operation
                });
            }
        }

        {
            int64_t partB = 0;

            for (auto &p : problems) {
                int64_t presult = p.operation == '*' ? 1 : 0;

                for (auto &f : p.factors) {
                    if (p.operation == '+') {
                        presult += std::stoll(f);

                    } else if (p.operation == '*') {
                        presult *= std::stoll(f);
                    }
                }

                partB += presult;
            }

            PRINTF(("PART_B: %lld", (long long int) partB));
        }
    }

	return 0;
}
