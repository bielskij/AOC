#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Program {
	std::string name;
	int         weight;

	std::vector<Program *> upper;
	Program               *bottom;

	Program() {
		this->weight = 0;
		this->bottom = nullptr;
	}

	Program(const std::string &name) : name(name) {
		this->weight = 0;
		this->bottom = nullptr;
	}

	Program(const std::string &name, int weight) : name(name) {
		this->weight = weight;
		this->bottom = nullptr;
	}

	int getWeight() const {
		int ret = this->weight;

		for (auto u : upper) {
			ret += u->getWeight();
		}

		return ret;
	}

	int ballance(int diff) {
		auto &u = this->upper;

		if (! u.empty()) {
			int lastIdx = u.size() - 1;

			std::sort(u.begin(), u.end(), [](Program *l, Program *r)->bool {
				return l->getWeight() < r->getWeight();
			});

			if (u[0]->getWeight() != u[1]->getWeight()) {
				return u[0]->ballance(u[1]->getWeight() - u[0]->getWeight());

			} else if (u[lastIdx]->getWeight() != u[lastIdx - 1]->getWeight()) {
				return u[lastIdx]->ballance(u[lastIdx - 1]->getWeight() - u[lastIdx]->getWeight());

			} else {
				int total = 0;

				for (auto c : u) {
					total += c->getWeight();
				}

				return this->getWeight() + diff - total;
			}
		}

		return 0;
	}
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<std::string, Program *> programs;

		std::string bottomProgram;

		for (auto const &line : lines) {
			auto words = utils::strTok(line, ' ');

			if (words.size() == 2) {
				auto name = words[0];

				if (programs[name] == nullptr) {
					programs[name] = new Program(name);
				}

				programs[name]->weight = utils::toInt(utils::trim(words[1], "()"));

			} else {
				auto src = words[0];

				if (programs[src] == nullptr) {
					programs[src] = new Program(src);
				}

				programs[src]->weight = utils::toInt(utils::trim(words[1], "()"));

				for (int i = 3; i < words.size(); i++) {
					std::string dst = words[i];

					if (*dst.rbegin() == ',') {
						dst = dst.substr(0, dst.size() - 1);
					}

					if (programs[dst] == nullptr) {
						programs[dst] = new Program(dst);
					}

					programs[src]->upper.push_back(programs[dst]);
					programs[dst]->bottom = programs[src];
				}
			}
		}

		for (const auto &p : programs) {
			if (p.second->bottom == nullptr) {
				bottomProgram = p.second->name;
				break;
			}
		}

		PRINTF(("PART_A: %s", bottomProgram.c_str()));
		PRINTF(("PART_B: %d", programs[bottomProgram]->ballance(0)));
	}

	return 0;
}
