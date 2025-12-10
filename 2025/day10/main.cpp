#include <deque>
#include <regex>

#include <unistd.h>
#include <sys/wait.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

struct Machine {
	uint16_t lightDiagram;
	uint16_t lightMask;
	std::vector<uint16_t> buttons;
	std::vector<int> joltage;
};

static std::string run(const std::vector<std::string> &args, const std::string &input) {
    int inPipe[2];
    int outPipe[2];

    pipe(inPipe);
    pipe(outPipe);

    pid_t pid = fork();
    if (pid == 0) {
        close(inPipe[1]);
        dup2(inPipe[0], STDIN_FILENO);
        close(inPipe[0]);

        close(outPipe[0]);
        dup2(outPipe[1], STDOUT_FILENO);
        close(outPipe[1]);

        std::vector<char *> cargs;

        for (auto& a : args) {
            cargs.push_back(const_cast<char *>(a.c_str()));
        }

        cargs.push_back(nullptr);

        execvp(cargs[0], cargs.data());
        _exit(1);
    }

    close(inPipe[0]);
    close(outPipe[1]);

    write(inPipe[1], input.data(), input.size());
    close(inPipe[1]);

    std::string output;

    {
        char buf[256];
        ssize_t n;

        while ((n = read(outPipe[0], buf, sizeof(buf))) > 0) {
            output.append(buf, n);
        }
    }

    close(outPipe[0]);

    {
        int status;
        
        waitpid(pid, &status, 0);
    }

    return output;
}

int main(int argc, char *argv[]) {
    auto lines = File::readAllLines(argv[1]);

    std::vector<Machine> machines;

    for (const auto &line : lines) {
        auto toks = utils::strTok(line, ' ');

        Machine machine;

        for (const auto &tok : toks) {
            switch (tok[0]) {
                case '[':
                    {
                        uint16_t indicators = 0;
                        uint16_t mask = 0;

                        for (int i = 1; i < tok.length() - 1; i++) {
                            if (tok[i] == '#') {
                                indicators |= (1 << (i - 1));
                            }

                            mask |= (1 << (i - 1));
                        }

                        machine.lightDiagram = indicators;
                        machine.lightMask    = mask;
                    }
                    break;

                case '(':
                    {
                        auto bitNumbers = utils::strTok(tok.substr(1, tok.length() - 2), ',');

                        uint16_t bitMask = 0;

                        for (const auto &bitNo : bitNumbers) {
                            bitMask |= (1 << std::stoi(bitNo));
                        }

                        machine.buttons.push_back(bitMask);
                    }
                    break;

                case '{':
                    {
                        auto joltage = utils::strTok(tok.substr(1, tok.length() - 2), ',');

                        for (const auto &tok : joltage) {
                            machine.joltage.push_back(std::stoi(tok));
                        }
                    }
                    break;

                default:
                    abort();
            }
        }

        machines.emplace_back(machine);
    }

    {
        int64_t partA = 0;

        for (auto &machine : machines) {
            // state, lastButton
            std::deque<uint16_t> q;

            std::map<uint16_t, std::pair<uint16_t, uint16_t>> par;

            q.push_back(0);

            par[0] = { 0, 0 };

            while (! q.empty()) {
                auto state = q.front(); q.pop_front();

                if (state == machine.lightDiagram) {
                    std::vector<uint16_t> seq;

                    while (state != 0) {
                        auto h = par[state];
                        seq.push_back(h.second);

                        state = h.first;
                    }

                    partA += seq.size();
                    break;
                }

                for (auto b : machine.buttons) {
                    uint16_t newState = (state ^ b) & machine.lightMask;

                    if (par.find(newState) == par.end()) {
                        par[newState] = { state, b };

                        q.push_back(newState);
                    }
                }
            }
        }

        PRINTF(("PART_A: %" PRId64, partA));
    }

    {
        int64_t partB = 0;

        for (const auto &machine : machines) {
            std::string script;

            script += "(reset)\n";

            std::string sumString;

            for (int i = 0; i < machine.buttons.size(); i++) {
                script += ("(declare-const b" + std::to_string(i) + " Int)\n");
                script += ("(assert (>= b" + std::to_string(i) + " 0))\n");

                sumString += (" b" + std::to_string(i));
            }

            for (int i = 0; i < machine.joltage.size(); i++) {
                std::string buttonsString;

                for (int j = 0; j < machine.buttons.size(); j++) {
                    if ((machine.buttons[j] & (1 << i)) != 0) {
                        buttonsString += (" b" + std::to_string(j));
                    }
                }

                script += ("(assert (= (+ " + buttonsString + ") " + std::to_string(machine.joltage[i]) + "))\n");
            }

            script += "(declare-const buttonsSum Int)\n";
            script += "(assert (= buttonsSum (+ " + sumString + ")))\n";
            script += "(minimize buttonsSum)\n";
            script += "(check-sat)\n";
            script += "(get-objectives)\n";

            auto resultString = run({ "z3", "-in" }, script);

            std::regex r(R"(\(buttonsSum\s+([0-9]+)\))");
            std::smatch m;

            if (std::regex_search(resultString, m, r)) {
                partB += std::stoi(m[1]);
            }
        }

        PRINTF(("PART_B: %" PRId64, partB));
    }

	return 0;
}