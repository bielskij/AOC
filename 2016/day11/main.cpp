#include <stdexcept>
#include <string.h>
#include <cassert>
#include <deque>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Floor {
	uint16_t microchips;
	uint16_t generators;

	Floor() {
		this->microchips = 0;
		this->generators = 0;
	}

	uint16_t getPairs() const {
		return this->microchips & this->generators;
	}

	uint16_t getDanglingMicrochips() const {
		return this->microchips & ~this->generators;
	}

	bool empty() const {
		return (this->microchips == 0) && (this->generators == 0);
	}

	bool hasGenerators() const {
		return this->generators != 0;
	}

	bool hasMicrochips() const {
		return this->microchips != 0;
	}

	void setMask(uint16_t mask, bool generators) {
		if (generators) {
			this->generators |= mask;
		} else {
			this->microchips |= mask;
		}
	}

	void clearMask(uint16_t mask, bool generators) {
		if (generators) {
			this->generators &= ~mask;
		} else {
			this->microchips &= ~mask;
		}
	}

	bool isCorrect() const {
		bool ret = true;

		if (ret) {
			if (this->generators != 0) {
				ret = (this->generators & this->microchips) == this->microchips;
			}
		}

		return ret;
	}
};


struct State {
	static constexpr int floorCount = 4;

	Floor floors[floorCount];

	int elevator;
	int steps;


	State() {
		this->elevator = 0;
		this->steps    = 0;
	}

	State(const State &other) {
		*this = other;
	}

	State &operator=(const State &other) {
		this->elevator = other.elevator;
		this->steps    = other.steps;

		for (int i = 0; i < floorCount; i++) {
			this->floors[i] = other.floors[i];
		}

		return *this;
	}

	uint64_t hash() {
		uint64_t ret = 0;

		{
			ret |= (this->elevator);

			for (int i = 0; i < floorCount; i++) {
				Floor &f = this->floors[i];

				ret <<= 7; ret |= f.generators;
				ret <<= 7; ret |= f.microchips;
			}
		}

		return ret;
	}

	bool finished() const {
		for (int i = 0; i < floorCount - 1; i++) {
			const Floor &f = this->floors[i];

			if (! f.empty()) {
				return false;
			}
		}

		return true;
	}

	bool isFirst() const {
		return this->elevator == 0;
	}

	bool isLast() const {
		return this->elevator == (this->floorCount - 1);
	}

	Floor *curr() {
		return &this->floors[this->elevator];
	}

	Floor *prev() {
		return &this->floors[this->elevator - 1];
	}

	Floor *next() {
		return &this->floors[this->elevator + 1];
	}

	void step(bool up) {
		this->elevator += (up ? 1 : -1);

		this->steps++;
	}
};


static int getNextIdx(uint32_t val, int startIdx) {
	int ret = -1;

	{
		uint32_t tmp = val;

		int offset = 0;

		if (startIdx >= 0) {
			offset = startIdx + 1;

			tmp >>= offset;
		}

		for (int i = 0; i < 32 - offset; i++) {
			if ((tmp & (1 << i)) != 0) {
				ret = i;
				break;
			}
		}

		if (ret >= 0) {
			ret += offset;
		}
	}

	return ret;
}


void valToVector(std::vector<int> &out, uint32_t val) {
	int idx = -1;

	out.clear();

	while ((idx = getNextIdx(val, idx)) >= 0) {
		out.push_back(idx);
	}
}

// first - generators, second - microchips
std::vector<std::pair<uint16_t, uint16_t>> getCombinations(Floor *src, Floor *dst, bool allowPairs) {
	std::vector<std::pair<uint16_t, uint16_t>> ret;

	std::vector<int> idxVector;

	idxVector.reserve(32);

	// microchip - generator pairs
	if (allowPairs) {
		valToVector(idxVector, (src->microchips << 16) | src->generators);

		if (idxVector.size() > 1) {
			std::vector<std::vector<int>> combinations;

			utils::genCombinations(combinations, idxVector.size(), 2);

			for (auto &c : combinations) {
				uint32_t pair = (1 << (idxVector[c[1] - 1])) | (1 << (idxVector[c[0] - 1]));

				ret.push_back(std::pair<uint16_t, uint16_t>(pair & 0xffff, pair >> 16));
			}
		}
	}

	if (src->hasMicrochips()) {
		uint16_t microchips = src->microchips;

		if (dst->hasGenerators()) {
			microchips &= dst->generators;
		}

		if (microchips != 0) {
			valToVector(idxVector, microchips);

			// Single microchip
			for (auto i : idxVector) {
				ret.push_back(std::pair<uint16_t, uint16_t>(0, 1 << i));
			}
		}
	}

	if (src->hasGenerators()) {
		uint16_t generators = src->generators;

		if (dst->getDanglingMicrochips() != 0) {
			generators &= dst->microchips;
		}

		if (generators != 0) {
			valToVector(idxVector, generators);

			// Single generator
			for (auto i : idxVector) {
				ret.push_back(std::pair<uint16_t, uint16_t>(1 << i, 0));
			}
		}
	}

	return ret;
}


static int solve(State &state) {
	std::deque<State> queue;
	std::map<uint64_t, bool> visited;

	visited[state.hash()] = true;

	queue.push_back(state);

	// BFS
	while (! queue.empty()) {
		State &state = queue.front();

		if (state.finished()) {
			return state.steps;
		}

		// Add all neighbours
		{
			// Backward
			if (! state.isFirst()) {
				auto combinations = getCombinations(state.curr(), state.prev(), false);

				for (auto &c : combinations) {
					State nextState = state;

					nextState.curr()->clearMask(c.first,  true);
					nextState.curr()->clearMask(c.second, false);

					nextState.prev()->setMask(c.first,  true);
					nextState.prev()->setMask(c.second, false);

					if (nextState.prev()->isCorrect() && nextState.curr()->isCorrect()) {
						nextState.step(false);

						uint64_t hash = nextState.hash();
						if (! visited[hash]) {
							visited[hash] = true;

							queue.push_back(std::move(nextState));
						}
					}
				}
			}

			// Forward
			if (! state.isLast()) {
				auto combinations = getCombinations(state.curr(), state.next(), true);

				for (auto &c : combinations) {
					State nextState = state;

					nextState.curr()->clearMask(c.first,  true);
					nextState.curr()->clearMask(c.second, false);

					nextState.next()->setMask(c.first,  true);
					nextState.next()->setMask(c.second, false);

					if (nextState.next()->isCorrect() && nextState.curr()->isCorrect()) {
						nextState.step(true);

						uint64_t hash = nextState.hash();
						if (! visited[hash]) {
							visited[hash] = true;

							queue.push_back(std::move(nextState));
						}
					}
				}
			}
		}

		queue.pop_front();
	}

	return -1;
}


int main(int argc, char *argv[]) {
	State state{};

	// Parse data
	{
		auto data = File::readAllLines(argv[1]);

		std::map<std::string, int> matterMap;
		int idx = 1;
		int floor = 0;

		for (auto &l : data) {
			size_t pos = 0;

			std::vector<std::string> words = utils::strTok(l, ' ');

			for (size_t i = 0; i < words.size(); i++) {
				std::string val = utils::trim(words[i], ".,");
				std::string matter;
				uint16_t *out = nullptr;

				if (val == "generator") {
					matter = words[i - 1];

					out = &state.floors[floor].generators;

				} else if (val == "microchip") {
					matter = words[i - 1];
					matter = matter.substr(0, matter.find('-'));

					out = &state.floors[floor].microchips;
				}

				if (out) {
					if (matterMap[matter] == 0) {
						matterMap[matter] = idx++;
					}

					*out |= (1 << (matterMap[matter] - 1));
				}
			}

			floor++;
		}
	}

	PRINTF(("PART_A: %d", solve(state)));

	state.floors[0].generators |= (1 << 5);
	state.floors[0].generators |= (1 << 6);
	state.floors[0].microchips |= (1 << 5);
	state.floors[0].microchips |= (1 << 6);

	PRINTF(("PART_B: %d", solve(state)));
}
