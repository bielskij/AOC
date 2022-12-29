#include <cassert>
#include <numeric>
#include <deque>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Blizzard {
	Point<int> position;
	char       direction;

	Blizzard(const Point<int> &pos, char direction) : position(pos) {
		this->direction = direction;
	}
};


struct BfsState {
	Point<int> position;
	int        minutes;

	BfsState(const Point<int> &pos, int m) : position(pos) {
		this->minutes = m;
	}

	bool operator<(const BfsState &other) const {
		return (this->position < other.position) || ((this->position == other.position) && (this->minutes < other.minutes));
	}
};


static int solve(const Point<int> &start, const Point<int> &end, const std::vector<std::vector<std::string>> &statesMap, int minute) {
	int ret = std::numeric_limits<int>::max();

	{
		std::deque<BfsState> queue;
		std::set<BfsState>   visited;

		queue.push_back(BfsState(start, minute));

		while (! queue.empty()) {
			BfsState state = queue.front(); queue.pop_front();

			if (state.minutes >= ret) {
				continue;
			}

			if (state.position == end) {
				ret = std::min(ret, state.minutes);
				continue;
			}

			if (visited.find(state) != visited.end()) {
				continue;
			}

			visited.insert(state);

			{
				Point<int> newPosition;

				const auto &stateMap = statesMap[(state.minutes + 1) % statesMap.size()];

				newPosition = { state.position.x() + 1, state.position.y() };
				if (newPosition.x() < stateMap[0].size() && stateMap[newPosition.y()][newPosition.x()] == '.') {
					queue.push_back(BfsState(newPosition, state.minutes + 1));
				}

				newPosition = { state.position.x(), state.position.y() + 1};
				if (newPosition.y() < stateMap.size() && stateMap[newPosition.y()][newPosition.x()] == '.') {
					queue.push_back(BfsState(newPosition, state.minutes + 1));
				}

				newPosition = { state.position.x() - 1, state.position.y() };
				if (newPosition.x() >= 0 && stateMap[newPosition.y()][newPosition.x()] == '.') {
					queue.push_back(BfsState(newPosition, state.minutes + 1));
				}

				newPosition = { state.position.x(), state.position.y() - 1 };
				if (newPosition.y() >= 0 && stateMap[newPosition.y()][newPosition.x()] == '.') {
					queue.push_back(BfsState(newPosition, state.minutes + 1));
				}

				newPosition = state.position;
				if (stateMap[newPosition.y()][newPosition.x()] == '.') {
					queue.push_back(BfsState(newPosition, state.minutes + 1));
				}
			}
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::vector<std::vector<std::string>> states;

		int rows        = lines.size();
		int cols        = lines[0].size();
		int cyclePeriod = utils::nww(rows - 2, cols - 2);

		{
			std::vector<Blizzard> blizzards;

			std::vector<std::string> emptyMap;

			for (int y = 0; y < lines.size(); y++) {
				auto line = lines[y];

				for (int x = 0; x < line.size(); x++) {
					char &c = line[x];

					if (c == 'v' || c == '^' || c == '<' || c == '>') {
						blizzards.push_back(Blizzard(Point<int>(x, y), c));

						c = '.';
					}
				}

				emptyMap.push_back(line);
			}

			for (int i = 0; i < cyclePeriod; i++) {
				std::vector<std::string> state = emptyMap;

				for (auto &b : blizzards) {
					state[b.position.y()][b.position.x()] = '#';
				}

				states.push_back(state);

				for (auto &b : blizzards) {
					switch (b.direction) {
						case '^':
							b.position.decY();
							if (b.position.y() <= 0) {
								b.position.y(rows - 2);
							}
							break;

						case 'v':
							b.position.incY();
							if (b.position.y() >= rows - 1) {
								b.position.y(1);
							}
							break;

						case '<':
							b.position.decX();
							if (b.position.x() <= 0) {
								b.position.x(cols - 2);
							}
							break;

						case '>':
							b.position.incX();
							if (b.position.x() >= cols - 1) {
								b.position.x(1);
							}
							break;
					}
				}
			}
		}

		{
			int partA;
			int partB;

			Point<int> start(lines.begin()->find('.'),  0);
			Point<int> end  (lines.rbegin()->find('.'), lines.size() - 1);

			partA = solve(start, end, states, 0);
			partB = solve(end, start, states, partA);
			partB = solve(start, end, states, partB);

			PRINTF(("PART_A: %d", partA));
			PRINTF(("PART_B: %d", partB));
		}
	}

	return 0;
}
