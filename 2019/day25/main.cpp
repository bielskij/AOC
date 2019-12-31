#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <unistd.h>
#include <stack>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchBfs.h"

#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class Droid : public IntCodeMachine {
	public:
		enum Direction {
			UNKNOWN,

			NORTH,
			EAST,
			WEST,
			SOUTH
		};

		struct Place {
			Place(const std::string &name) : name(name) {
			}

			Place() {
			}

			std::string name;
			std::string description;

			// direction, place
			std::vector<Direction>   nextDoors;
			std::vector<std::string> items;
		};

	private:
		enum ParserState {
			IDLE,

			PLACE_DOORS,
			PLACE_ITEMS
		};

		enum ExpectedResponse {
			PLACE,
			ITEM_TAKEN,
			ITEM_DROPPED
		};

	public:
		Droid(const std::vector<int64_t> &program) : IntCodeMachine(program) {
			this->commandWritten  = 0;
			this->commandExpected = ExpectedResponse::PLACE;
			this->commandSuccess  = false;
		}

		void save(SaveSlot &slot) {
			this->savedPlace = this->currentPlace;

			this->IntCodeMachine::save(slot);
		}


		void load(SaveSlot &slot) {
			this->currentPlace = this->savedPlace;

			this->IntCodeMachine::load(slot);
		}

		void parsePlace() {
			ParserState state = IDLE;

			for (auto l : this->lines) {
				if (l.empty() || l.compare("\n") == 0) {
					state = ParserState::IDLE;

					continue;
				}

				switch (state) {
					case ParserState::IDLE:
						if (l.compare("Doors here lead:") == 0) {
							state = ParserState::PLACE_DOORS;

						} else if (l.compare("Items here:") == 0) {
							state = ParserState::PLACE_ITEMS;

						} else if ((*l.begin() == '=') && (*l.rbegin() == '=')) {
							this->currentPlace.name = l.substr(3, l.length() - 6);

							this->commandSuccess = true;

						} else if (l.compare("Command?") == 0) {
							// nothing to do

						} else {
							this->currentPlace.description += l;
						}
						break;

					case ParserState::PLACE_DOORS:
						{
							this->currentPlace.nextDoors.push_back(stringToDirection(l.substr(2)));
						}
						break;

					case ParserState::PLACE_ITEMS:
						{
							this->currentPlace.items.push_back(l.substr(2));
						}
						break;
				}
			}
		}

		bool onOut(int64_t value) {
			if (value == '\n') {
				this->lines.push_back(this->line);
				this->line.clear();

			} else {
				this->line.push_back(value);
			}

//			printf("%c", value);

			return true;
		}

		bool onIn(int64_t &value) {
			bool ret = false;

			if (this->command.empty()) {
				// Command sent, response received and machine is waiting for the next command
				if (this->commandExpected == ExpectedResponse::PLACE) {
					this->parsePlace();

				} else if (this->commandExpected == ExpectedResponse::ITEM_TAKEN) {
					for (auto &l : this->lines) {
						if (l.find("You take the") != std::string::npos) {
							this->commandSuccess = true;
							break;
						}
					}

				} else if (this->commandExpected == ExpectedResponse::ITEM_DROPPED) {
					for (auto &l : this->lines) {
						if (l.find("You drop the") != std::string::npos) {
							this->commandSuccess = true;
							break;
						}
					}
				}

				if (this->commandSuccess) {
					this->commandSuccess = false;

					for (auto &l : this->lines) {
						if (l.compare("Command?") == 0) {
							this->commandSuccess = true;
						}
					}
				}

			} else {
				// Write command
				if (this->commandWritten == this->command.length()) {
					value = '\n';

					this->commandWritten = 0;
					this->command.clear();

				} else {
					value = this->command.at(commandWritten++);
				}

				ret = true;
			}

			return ret;
		}

		static std::string directionToString(Direction dir) {
			std::string ret;

			switch (dir) {
				case Direction::UNKNOWN: abort(); break;

				case Direction::EAST:  ret = "east";  break;
				case Direction::WEST:  ret = "west";  break;
				case Direction::SOUTH: ret = "south"; break;
				case Direction::NORTH: ret = "north"; break;
			}

			return ret;
		}

		static Direction stringToDirection(const std::string &str) {
			Direction ret = Direction::UNKNOWN;

			if (str.compare("east") == 0) {
				ret = Direction::EAST;
			} else if (str.compare("west") == 0) {
				ret = Direction::WEST;
			} else if (str.compare("north") == 0) {
				ret = Direction::NORTH;
			} else if (str.compare("south") == 0) {
				ret = Direction::SOUTH;
			}

			return ret;
		}

		static Direction oppositeDirection(Direction dir) {
			Direction ret = Direction::UNKNOWN;

			switch (dir) {
				case Direction::EAST:  ret = Direction::WEST;  break;
				case Direction::WEST:  ret = Direction::EAST;  break;
				case Direction::NORTH: ret = Direction::SOUTH; break;
				case Direction::SOUTH: ret = Direction::NORTH; break;
			}

			return ret;
		}

		bool goDirection(Direction dir, bool &returnedBack) {
			std::string currentPlace = this->currentPlace.name;

			this->command         = directionToString(dir);
			this->commandExpected = ExpectedResponse::PLACE;
			this->commandSuccess  = false;
			this->commandWritten  = 0;

			this->currentPlace = Place();

			this->lines.clear();
			this->line.clear();

			this->run();

			// At last point program finishes
			if (this->finished()) {
				this->parsePlace();
			}

			if (this->commandSuccess) {
				if (this->currentPlace.name.compare(currentPlace) == 0) {
					returnedBack = true;

				} else {
					returnedBack = false;
				}
			}

			return this->commandSuccess;
		}

		bool takeItem(const std::string &item) {
			this->command         = "take " + item;
			this->commandExpected = ExpectedResponse::ITEM_TAKEN;
			this->commandSuccess  = false;
			this->commandWritten  = 0;

			this->lines.clear();
			this->line.clear();

			this->run();

			return this->commandSuccess;
		}

		bool dropItem(const std::string &item) {
			this->command         = "drop " + item;
			this->commandExpected = ExpectedResponse::ITEM_DROPPED;
			this->commandSuccess  = false;
			this->commandWritten  = 0;

			this->lines.clear();
			this->line.clear();

			this->run();

			return this->commandSuccess;
		}

		void init() {
			this->run();
		}

		Place *getCurrentPlace() {
			return &this->currentPlace;
		}

	private:
		std::string      command;
		int              commandWritten;
		ExpectedResponse commandExpected;
		bool             commandSuccess;


		Place currentPlace;
		Place savedPlace;

		std::string line;
		std::vector<std::string> lines;
};


struct PlaceNode : public graph::Node {
	std::string name;
	std::string desc;
	PlaceNode      *parent;

	std::map<Droid::Direction, PlaceNode *> children;

	static int idx;

	PlaceNode() {
		this->parent = nullptr;
	}

	PlaceNode(Droid::Place *place, PlaceNode *parent) : graph::Node(idx++), name(place->name), desc(place->description) {
		this->parent = parent;

		for (auto &d : place->nextDoors) {
			this->children[d] = nullptr;
		}
	}

	Droid::Direction getNextDirection() {
		Droid::Direction ret = Droid::Direction::UNKNOWN;

		for (auto &d : this->children) {
			if (d.second == nullptr) {
				ret = d.first;
				break;
			}
		}

		return ret;
	}
};

int PlaceNode::idx = 0;


struct PlaceEdge : public graph::Edge {
	PlaceEdge(PlaceNode *src, PlaceNode *dst, Droid::Direction dir) : graph::Edge(src, dst) {
		this->direction = dir;
	}

	Droid::Direction direction;
};


int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	Droid d(program);

	d.init();

	std::vector<PlaceEdge *>           edges;
	std::map<std::string, PlaceNode *> places;  // all visited places
	std::vector<std::string>           items;   // collected items
	std::stack<Droid::Direction>       history; // Move history

	std::vector<std::string> ignoredItems = {
		"infinite loop",
		"giant electromagnet"
	};

	std::string      enterPoint;
	Droid::Direction enterPointDirection;

	PlaceNode *currentPlace = places[d.getCurrentPlace()->name] = new PlaceNode(d.getCurrentPlace(), nullptr);
	do {
		Droid::Direction nextDir = currentPlace->getNextDirection();

		bool returnedBack;

		// Go back if no way available
		if (nextDir == Droid::Direction::UNKNOWN) {
			if (! d.goDirection(Droid::oppositeDirection(history.top()), returnedBack)) {
				PRINTF(("Cannot go: %s (history)", Droid::directionToString(Droid::oppositeDirection(history.top())).c_str()));
				break;
			}

			history.pop();

			currentPlace = places[d.getCurrentPlace()->name];
			continue;
		}

		if (! d.goDirection(nextDir, returnedBack)) {
			PRINTF(("Cannot go to %s", Droid::directionToString(nextDir).c_str()));
			break;
		}

		if (! returnedBack) {
			history.push(nextDir);

		} else {
			enterPointDirection = nextDir;
			enterPoint          = currentPlace->name;
		}

		{
			Droid::Place *nextPlace = d.getCurrentPlace();

			if (places[nextPlace->name] == nullptr) {
				places[nextPlace->name] = new PlaceNode(nextPlace, currentPlace);
			}

			edges.push_back(new PlaceEdge(currentPlace, places[nextPlace->name], nextDir));

			places[currentPlace->name]->children[nextDir] = places[nextPlace->name];

			currentPlace = places[nextPlace->name];

			for (auto &i : nextPlace->items) {
				IntCodeMachine::SaveSlot slot;

				if (std::find(items.begin(), items.end(), i) == items.end()) {
					if (std::find(ignoredItems.begin(), ignoredItems.end(), i) != ignoredItems.end()) {
						continue;
					}

					d.save(slot);

					if (! d.takeItem(i)) {
						d.load(slot);

					} else {
						items.push_back(i);
					}
				}
			}
		}
	} while (! history.empty());

//	for (auto &p : places) {
//		PRINTF(("Place: %s, description: %s", p.second->name.c_str(), p.second->desc.c_str()));
//	}
//
//	for (auto &i : items) {
//		PRINTF(("Item: %s", i.c_str()));
//	}

	PRINTF(("Enter point: %s, direction: %s", enterPoint.c_str(), Droid::directionToString(enterPointDirection).c_str()));

	// Go to entry point
	{
		graph::Graph g;

		for (auto &p : places) {
			g.addNode(p.second);
		}

		for (auto &e : edges) {
			g.addValidEdge(e);
		}

		graph::GraphSearchBfs bfs(&g);

		std::vector<graph::Node *> route;

		bfs.search(places[d.getCurrentPlace()->name]->id(), places[enterPoint]->id(), route, false);

		for (size_t i = 1; i < route.size(); i++) {
			bool ignored;

			d.goDirection(reinterpret_cast<PlaceEdge *>(g.getEdge(route[i - 1]->id(), route[i]->id()))->direction, ignored);
		}
	}

	// Check all item combinations
	{
		IntCodeMachine::SaveSlot savedState;

		d.save(savedState);

		for (int i = 1; i <= items.size(); i++) {
			std::vector<std::vector<int>> combinations;

			utils::genCombinations(combinations, items.size(), i);

			// Drop items
			for (auto &c : combinations) {
				d.load(savedState);

				for (auto &e : c) {
					d.dropItem(items.at(e - 1));
				}

				bool returnedBack;
				if (d.goDirection(enterPointDirection, returnedBack) && !returnedBack) {
					PRINTF(("Has correct combination! \n\n%s", d.getCurrentPlace()->description.c_str()));

					exit(1);
					break;
				}
			}
		}
	}
}
