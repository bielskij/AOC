#include <assert.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static void rotate(std::vector<std::string> &map) {
	std::vector<std::string> newMap;

	for (size_t i = 0; i < map[0].size(); i++) {
		std::string newRow;

		for (int j = map.size() - 1; j >= 0; j--) {
			newRow.push_back(map[j][i]);
		}

		newMap.push_back(newRow);
	}

	map = newMap;
}


static void flip(std::vector<std::string> &map) {
	for (auto &r : map) {
		std::reverse(r.begin(), r.end());
	}
}


struct Fragment {
	enum Edge {
		TOP,
		RIGHT,
		BOTTOM,
		LEFT,

		COUNT
	};

	Fragment() {
		this->reset();
	}

	void reset() {
		this->map.clear();

		this->id = -1;
	}

	int getWidth() const {
		return this->map[0].length();
	}

	int getHeight() const {
		return this->map.size();
	}

	std::string getEdge(Edge edge) {
		std::string ret;

		switch (edge) {
			case Edge::TOP:
				ret = *this->map.begin();
				break;

			case Edge::BOTTOM:
				ret = *this->map.rbegin();
				break;

			case Edge::RIGHT:
				for (auto &row : map) {
					ret.push_back(*row.rbegin());
				}
				break;

			case Edge::LEFT:
				for (auto &row : map) {
					ret.push_back(*row.begin());
				}
				break;
		}

		return ret;
	}

	void flip() {
		::flip(this->map);
	}

	void rotate() {
		::rotate(this->map);
	}

	int id;
	std::vector<std::string> map;

};


struct Puzzle {
	static void fillRow(std::map<std::string, std::set<Fragment *>> &map, std::vector<Fragment *> &out) {
		Fragment *current = *out.begin();
		do {
			Fragment *neighbour = nullptr;
			for (auto n : map[current->getEdge(Fragment::RIGHT)]) {
				if (n != current) {
					neighbour = n;
					break;
				}
			}

			if (neighbour) {
				int i = 0;

				while (
					neighbour->getEdge(Fragment::LEFT) != current->getEdge(Fragment::RIGHT) &&
					i < 4
				) {
					neighbour->rotate();

					i++;
				}

				if (i == 4) {
					neighbour->flip();

					i = 0;
					while (
						neighbour->getEdge(Fragment::LEFT) != current->getEdge(Fragment::RIGHT) &&
						i < 4
					) {
						neighbour->rotate();

						i++;
					}
				}

				assert(neighbour->getEdge(Fragment::LEFT) == current->getEdge(Fragment::RIGHT));

				out.push_back(neighbour);
			}

			current = neighbour;
		} while (current != nullptr);
	}

	static int findMonsters(std::vector<std::string> &map, std::vector<std::string> &monster) {
		int matches = 0;

		for (int row = 2; row < map.size(); row++) {
			for (int col = 0; col < map[row].length() - monster[0].length(); col++) {
				bool matched = true;

				for (int x = 0; x < monster[0].length() && matched; x++) {
					for (int y = 0; y < monster.size(); y++) {
						if (monster[monster.size() - y - 1][x] == '#' && map[row - y][col + x] != '#') {
							matched = false;
							break;
						}
					}
				}

				if (matched) {
					matches++;
				}
			}
		}

		return matches;
	}

	int64_t getPartA() {
		int64_t ret = 1;

		ret *= (*this->solvedPuzzles.begin()->begin())->id;
		ret *= (*this->solvedPuzzles.begin()->rbegin())->id;
		ret *= (*this->solvedPuzzles.rbegin()->begin())->id;
		ret *= (*this->solvedPuzzles.rbegin()->rbegin())->id;

		return ret;
	}

	int64_t getPartB() {
		// Generate string representation
		std::vector<std::string> solvedPuzzlesString;

		for (int row = 0; row < this->solvedPuzzles.size(); row++) {
			std::vector<Fragment *>::iterator fragmentIt = this->solvedPuzzles[row].begin();

			int w = (*fragmentIt)->getWidth();
			int h = (*fragmentIt)->getHeight();

			// Add rows
			for (int hi = 1; hi < h - 1; hi++) {
				solvedPuzzlesString.push_back("");
			}

			while (fragmentIt != this->solvedPuzzles[row].end()) {
				for (int wi = 1; wi < w - 1; wi++) {
					for (int hi = 1; hi < h - 1; hi++) {
						solvedPuzzlesString[row * (h - 2) + hi - 1].push_back((*fragmentIt)->map[hi][wi]);
					}
				}

				fragmentIt++;
			}
		}

//		for (auto &row : solvedPuzzlesString) {
//			PRINTF(("%s", row.c_str()));
//		}
//		PRINTF(("\n"));

		// Find monsters
		{
			std::vector<std::string> monster;

			monster.push_back("                  # ");
			monster.push_back("#    ##    ##    ###");
			monster.push_back(" #  #  #  #  #  #   ");

			int monsterHashes = 0;
			int puzzleHashes  = 0;

			int partB = 0;

			for (auto &row : monster) {
				monsterHashes += std::count(row.begin(), row.end(), '#');
			}

			for (auto &row : solvedPuzzlesString) {
				puzzleHashes += std::count(row.begin(), row.end(), '#');
			}

			for (int turn = 0; turn < 4; turn++) {
				int matches;

				matches = findMonsters(solvedPuzzlesString, monster);
				if (matches) {
					partB += (puzzleHashes - monsterHashes * matches);
				}

				flip(solvedPuzzlesString);

				matches = findMonsters(solvedPuzzlesString, monster);
				if (matches) {
					partB += (puzzleHashes - monsterHashes * matches);
				}

				flip(solvedPuzzlesString);

				rotate(solvedPuzzlesString);
			}

			return partB;
		}
	}

	Puzzle(std::vector<Fragment *> &fragments) {
		std::map<std::string, std::set<Fragment *>> map;

		for (auto fragment : fragments) {
			for (int edge = 0; edge < Fragment::Edge::COUNT; edge++) {
				std::string edgeStr = fragment->getEdge((Fragment::Edge) edge);

				map[edgeStr].insert(fragment);

				std::reverse(edgeStr.begin(), edgeStr.end());

				map[edgeStr].insert(fragment);
			}
		}

		Fragment *corner = nullptr;

		// Find corners
		for (auto fragment : fragments) {
			int stickyEdges = 4;

			for (int edge = 0; edge < Fragment::Edge::COUNT; edge++) {
				std::string edgeStr = fragment->getEdge((Fragment::Edge) edge);

				if (map[edgeStr].find(fragment) != map[edgeStr].end() && map[edgeStr].size() > 1) {
					stickyEdges--;
				}
			}

			if (stickyEdges == 2) {
				if (corner == nullptr) {
					corner = fragment;
				}
			}
		}

		// Fill first col
		{
			// make the corner top-left
			while (
				map[corner->getEdge(Fragment::TOP)].size()  != 1 ||
				map[corner->getEdge(Fragment::LEFT)].size() != 1
			) {
				corner->rotate();
			}

			// Fill first column
			this->solvedPuzzles.push_back(std::vector<Fragment *>());
			this->solvedPuzzles.rbegin()->push_back(corner);

			Fragment *current = corner;
			do {
				Fragment *neighbour = nullptr;
				for (auto n : map[current->getEdge(Fragment::BOTTOM)]) {
					if (n != current) {
						neighbour = n;
						break;
					}
				}

				if (neighbour) {
					int i = 0;

					while (
						neighbour->getEdge(Fragment::TOP) != current->getEdge(Fragment::BOTTOM) &&
						i < 4
					) {
						neighbour->rotate();

						i++;
					}

					if (i == 4) {
						neighbour->flip();

						i = 0;
						while (
							neighbour->getEdge(Fragment::TOP) != current->getEdge(Fragment::BOTTOM) &&
							i < 4
						) {
							neighbour->rotate();

							i++;
						}
					}

					assert(neighbour->getEdge(Fragment::TOP) == current->getEdge(Fragment::BOTTOM));

					this->solvedPuzzles.push_back(std::vector<Fragment *>());
					this->solvedPuzzles.rbegin()->push_back(neighbour);
				}

				current = neighbour;

			} while (current != nullptr);

			// Fill all rows
			for (auto &row : this->solvedPuzzles) {
				fillRow(map, row);
			}
		}
	}

	std::vector<std::vector<Fragment *>> solvedPuzzles;
};


enum State {
	STATE_ID,
	STATE_MAP
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	std::vector<Fragment *> fragments;

	{
		Fragment *tmp = nullptr;
		bool nextIsId = true;

		for (auto &l : lines) {
			if (l.empty()) {
				nextIsId = true;

				if (tmp) {
					fragments.push_back(tmp);

					tmp = nullptr;
				}

				continue;
			}

			if (nextIsId) {
				tmp = new Fragment();

				sscanf(l.c_str(), "Tile %d:", &tmp->id);

				nextIsId = false;

			} else {
				tmp->map.push_back(l);
			}
		}

		if (tmp != nullptr) {
			fragments.push_back(tmp);
		}
	}

	{
		Puzzle puzzle(fragments);

		PRINTF(("PART_A: %" PRId64, puzzle.getPartA()));
		PRINTF(("PART_B: %" PRId64, puzzle.getPartB()));
	}

	return 0;
}
