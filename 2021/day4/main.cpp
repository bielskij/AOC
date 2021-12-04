#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class Board {
	public:
		Board() {

		}

		void addRow(const std::vector<int> &numbers) {
			this->numbers.insert(this->numbers.end(), numbers.begin(), numbers.end());
		}

		void dump() const {
			int len = std::sqrt(this->numbers.size());

			for (int row = 0; row < len; row++) {
				for (int col = 0; col < len; col++) {
					printf("%d ", this->numbers[row * len + col]);
				}
				printf("\n");
			}
		}

		bool isWinner(const std::vector<int> &numbers) const {
			std::map<int, std::set<int>> hits;

			int boardLen = std::sqrt(this->numbers.size());

			for (int num : numbers) {
				auto posIt = std::find(this->numbers.begin(), this->numbers.end(), num);
				if (posIt != this->numbers.end()) {
					int offset = std::distance(this->numbers.begin(), posIt);

					hits[offset / boardLen].insert(offset % boardLen);
				}
			}

			for (const auto &row : hits) {
				if (row.second.size() == boardLen) {
					return true;
				}
			}

			if (hits.size() == boardLen) {
				for (int val : hits.begin()->second) {
					int rows = 0;

					for (auto const &row : hits) {
						if (row.second.find(val) != row.second.end()) {
							rows++;
						}
					}

					if (rows == boardLen) {
						return true;
					}
				}
			}

			return false;
		}

		int64_t getPartA(const std::vector<int> &numbers) const {
			int64_t ret = 0;

			{
				std::set<int> cp(this->numbers.begin(), this->numbers.end());

				for (auto num : numbers) {
					cp.erase(num);
				}

				for (auto num : cp) {
					ret += num;
				}
			}

			return ret * *numbers.rbegin();
		}

	private:
		std::vector<int> numbers;
//		std::vector<std::vector<int>> numbers;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<Board> bingo;

		std::vector<int> numbers = utils::toIntV(utils::strTok(lines[0], ','));

		for (size_t i = 1; i < lines.size(); i++) {
			const std::string &l = lines[i];
			if (l.empty()) {
				bingo.push_back(Board());

			} else {
				auto tokens = utils::strTok(l, ' ');

				std::vector<int> row;

				for (const auto &token : tokens) {
					std::string s = utils::trim(token, " ");
					if (! s.empty()) {
						row.push_back(utils::toInt(s));
					}
				}

				bingo.rbegin()->addRow(row);
			}
		}
#if 0
		for (auto const &board : bingo) {
			board.dump();
			printf("\n");
		}
#endif
		for (int i = 5; i < numbers.size(); i++) {
			Board const *winner = nullptr;

			std::vector<int> turn(numbers.begin(), numbers.begin() + i);

			for (auto const &board : bingo) {
				if (board.isWinner(turn)) {
					winner = &board;
					break;
				}
			}

			if (winner != nullptr) {
				PRINTF(("PART_A: %ld", winner->getPartA(turn)));
//				winner->dump();
				break;
			}
		}

		for (int i = 5; i < numbers.size(); i++) {
			std::vector<int> turn(numbers.begin(), numbers.begin() + i);

			std::vector<Board>::iterator it = bingo.begin();
			while (it != bingo.end()) {
				if (it->isWinner(turn)) {
					if (bingo.size() == 1) {
						PRINTF(("PART_B: %ld", it->getPartA(turn)));
					}

					it = bingo.erase(it);

				} else {
					it++;
				}
			}
		}
	}

	return 0;
}
