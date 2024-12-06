#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

static const std::vector<Direction> turnMap = {
	RIGHT,
	LEFT,
	UP,
	DOWN
};

static const std::array<std::pair<int, int>, 4> directionModifier = {{
	{  0, -1 },
	{  0,  1 },
	{ -1,  0 },
	{  1,  0 }
}};


struct Map {
	public:
		Map(int width, int height) {
			this->width  = width;
			this->height = height;
		}

		void addObstruction(const Point<int> &p) {
			this->xy[p.x()].push_back(p.y());
			this->yx[p.y()].push_back(p.x());
		}

		bool isObstruction(const Point<int> &p) const {
			auto xIt = this->xy.find(p.x());
			if (xIt != this->xy.end()) {
				if (std::find(xIt->second.begin(), xIt->second.end(), p.y()) != xIt->second.end()) {
					return true;
				}
			}
			
			return false;
		}

		int getWidth() const {
			return this->width;
		}

		int getHeight() const {
			return this->height;
		}

	private:
		std::map<int, std::vector<int>> xy;
		std::map<int, std::vector<int>> yx;

		int width;
		int height;
};

struct State {
	private:
		Point<int>                      poisition;
		Direction                       direction;
		std::map<Point<int>, Direction> turnHistory;
		std::set<Point<int>>            steps;
		bool                            inLoop;

	public:
		State() {
			this->direction = Direction::UP;
			this->inLoop    = false;
		}

		bool step(const Map &map) {
			bool ret = ! this->isInLoop();

			if (ret) {
				auto nextPos = this->poisition;

				this->steps.insert(this->poisition);

				nextPos.x(nextPos.x() + directionModifier[this->direction].first);
				nextPos.y(nextPos.y() + directionModifier[this->direction].second);

				if (map.isObstruction(nextPos)) {
					auto it = this->turnHistory.find(nextPos);
					if (it != this->turnHistory.end()) {
						if (it->second == this->direction) {
							this->inLoop = true;
						}
					}

					if (! this->inLoop) {
						this->turnHistory[nextPos] = this->direction;

						this->direction = turnMap[this->direction];
					}

				} else if (
					(nextPos.x() < 0 || nextPos.x() >= map.getWidth()) ||
					(nextPos.y() < 0 || nextPos.y() >= map.getHeight())
				) {
					ret = false;
					
				} else {
					this->poisition = nextPos;
				}
			}

			return ret;
		}

		bool isInLoop() const {
			return this->inLoop;
		}

		void setDirection(Direction dir) {
			this->direction = dir;
		}

		void setPosition(const Point<int> &p) {
			this->poisition = p;
		}

		const std::set<Point<int>> &getSteps() const {
			return this->steps;
		}

		void clear() {
			this->turnHistory.clear();
			this->steps.clear();
			this->inLoop = false;
		}
};

int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		Map map(lines[0].length(), lines.size());
		State initialState;

		// Parse map
		for (int y = 0; y < lines.size(); y++) {
			const auto &row = lines[y];

			for (int x = 0; x < row.length(); x++) {
				auto p = Point<int>(x, y);

				switch (row[x]) {
					case '^':
						initialState.setDirection(Direction::UP);
						initialState.setPosition(p);
						break;

					case '#':
						map.addObstruction(p);
						break;

					default:
						break;
				}
			}
		}

		{
			State state = initialState;

			while (state.step(map)) {
			}

			PRINTF(("PART_A: %zd", state.getSteps().size()));
		}
	}
	

	return 0;
}
