#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>
#include <unistd.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class Network {
	public:
		virtual ~Network() {
		}

		virtual void onPacket(int64_t address, int64_t x, int64_t y) = 0;
};

class Computer : public IntCodeMachine {
	public:
		enum OutType {
			OUT_ADDRESS,
			OUT_X,
			OUT_Y
		};

		enum InType {
			IN_X,
			IN_Y
		};

	public:
		Computer(const std::vector<int64_t> &program, int address, Network *network) : IntCodeMachine(program) {
			this->network = network;
			this->address = address;

			this->addressBind = false;
			this->outType     = OutType::OUT_ADDRESS;
			this->inType      = InType::IN_X;
			this->idle        = false;
		}

		bool onOut(int64_t value) {
			this->buffer[this->outType] = value;

			switch (this->outType) {
				case OutType::OUT_ADDRESS:
					{
						this->outType = OutType::OUT_X;
					}
					break;

				case OutType::OUT_X:
					{
						this->outType = OutType::OUT_Y;
					}
					break;

				case OutType::OUT_Y:
					{
						this->outType = OutType::OUT_ADDRESS;

						this->network->onPacket(this->buffer[0], this->buffer[1], this->buffer[2]);
					}
					break;
			}

			return true;
		}

		bool onIn(int64_t &value) {
			if (! this->addressBind) {
				value = this->address;

				this->addressBind = true;

			} else {
				if (this->inQueue.empty()) {
					this->idle = true;

					value = -1;

				} else {
					this->idle = false;

					if (this->inType == InType::IN_X) {
						value = this->inQueue.front().first;

						this->inType = InType::IN_Y;

					} else {
						value = this->inQueue.front().second;

						this->inType = InType::IN_X;
						this->inQueue.pop();
					}
				}
			}

			return true;
		}

		void onPacket(int64_t x, int64_t y) {
			this->inQueue.push(std::pair<int64_t, int64_t>(x, y));
		}

		bool isIdle() const {
			return this->idle;
		}

	private:
		int address;
		Network *network;

		OutType outType;
		InType  inType;

		bool addressBind;

		int64_t buffer[3];
		bool    idle;

		std::queue<std::pair<int64_t, int64_t>> inQueue;
};


class ComputerNetwork : public Network {
	public:
		ComputerNetwork(std::map<int, Computer *> *computers) {
			this->computers         = computers;
			this->natPacketReceived = false;
			this->partAreported     = false;
		}

		void onPacket(int64_t address, int64_t x, int64_t y) {
			if (address == 255) {
				this->natPacket[0] = x;
				this->natPacket[1] = y;

				this->natPacketReceived = true;

				if (! this->partAreported) {
					PRINTF(("PART_A: %d", (int) y));

					this->partAreported = true;
				}

			} else {
				this->computers->at(address)->onPacket(x, y);
			}
		}

		void step() {
			if (this->natPacketReceived) {
				bool isIdle = true;

				for (auto &c : *this->computers) {
					if (! c.second->isIdle()) {
						isIdle = false;
						break;
					}
				}

				if (isIdle) {
					this->computers->at(0)->onPacket(this->natPacket[0], this->natPacket[1]);

					this->natPacketReceived = false;

					this->lastY[0] = this->lastY[1];
					this->lastY[1] = this->natPacket[1];

					if (this->lastY[0] == this->lastY[1]) {
						PRINTF(("PART_B: %d", this->lastY[0]));
					}
				}
			}
		}

	private:
		std::map<int, Computer *> *computers;

		int64_t natPacket[2];
		bool    natPacketReceived;
		int64_t lastY[2];
		bool    partAreported;
};


int main(int argc, char *argv[]) {
	std::vector<int64_t> program = utils::toInt64tV(utils::strTok(File::readAllLines(argv[1])[0], ','));

	{
		std::map<int, Computer *> computers;
		ComputerNetwork net(&computers);

		for (int i = 0; i < 50; i++) {
			computers[i] = new Computer(program, i, &net);
		}

		bool allFinished = false;
		while (! allFinished) {
			for (auto &c : computers) {
				c.second->step();

				net.step();
			}

			allFinished = true;
			for (auto &c : computers) {
				if (! c.second->finished()) {
					allFinished = false;
					break;
				}
			}
		}
	}
}
