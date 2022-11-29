#include <functional>
#include <memory>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Coprocessor {
	class Reg {
		public:
			Reg() {
				this->reset();
			}

			void reset() {
				this->_val = 0;
			}

			int64_t val() const {
				return this->_val;
			}

			void val(int64_t v) {
				this->_val = v;
			}

		private:
			int64_t _val;
	};

	class Regs {
		public:
			Regs() {
				for (int i = 0; i < 8; i++) {
					this->_regs.push_back(Reg());
				}
			}

			Reg &fromChar(char c) {
				if (c < 'a' || c > 'h') {
					abort();
				}
				return _regs[c - 'a'];
			}

			void reset() {
				for (auto &r : this->_regs) {
					r.reset();
				}
			}

		private:
			std::vector<Reg> _regs;
	};

	class Operand {
		public:
			virtual ~Operand() {}

		public:
			virtual bool isReg() const = 0;
			virtual int64_t val() const  = 0;
			virtual void val(int64_t v) = 0;
	};

	class RegOperand : public Operand {
		public:
			RegOperand(Reg &r) : r(r) {
			}

			virtual bool isReg() const override {
				return true;
			}

			virtual int64_t val() const override {
				return this->r.val();
			}

			virtual void val(int64_t v) override {
				this->r.val(v);
			}

		private:
			Reg &r;
	};

	class ImmOperand : public Operand {
		public:
			ImmOperand(int64_t val) {
				this->_v = val;
			}

			virtual bool isReg() const override {
				return false;
			}

			virtual int64_t val() const override {
				return this->_v;
			}

			virtual void val(int64_t v) override {
				abort();
			}

		private:
			int64_t _v;
	};

	Regs regs;
	int pc;
	std::vector<std::string> code;
	std::map<std::string, std::function<int(Operand &, const Operand &)>> opcodes;

	Coprocessor() {
		opcodes["set"] = [this](Operand &x, const Operand &y) {
			return this->set(x, y);
		};

		opcodes["sub"] = [this](Operand &x, const Operand &y) {
			return this->sub(x, y);
		};

		opcodes["mul"] = [this](Operand &x, const Operand &y) {
			return this->mul(x, y);
		};

		opcodes["jnz"] = [this](Operand &x, const Operand &y) {
			return this->jnz(x, y);
		};

		this->reset();
	}

	virtual ~Coprocessor() {}

	void load(const std::vector<std::string> &code) {
		this->code = code;
	}

	void reset() {
		this->regs.reset();

		this->pc = 0;
	}

	void run() {
		while (this->pc < this->code.size()) {
			std::unique_ptr<Operand> x;
			std::unique_ptr<Operand> y;

			std::string opName;

			{
				auto tokens = utils::strTok(this->code[this->pc], ' ');

				if (tokens.size() != 3) {
					PRINTF(("Unable to parse instruction! '%s'", this->code[this->pc].c_str()));
					exit(1);
				}

//				DBG(("[%d] opcode: '%s', left: '%s', right: '%s'", this->pc, tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str()));

				{
					auto &opLeft = tokens[1];

					if (::isalpha(opLeft[0])) {
						x = std::make_unique<RegOperand>(this->regs.fromChar(opLeft[0]));
					} else {
						x = std::make_unique<ImmOperand>(std::stol(opLeft));
					}
				}

				{
					auto &opRight = tokens[2];

					if (::isalpha(opRight[0])) {
						y = std::make_unique<RegOperand>(this->regs.fromChar(opRight[0]));
					} else {
						y = std::make_unique<ImmOperand>(std::stol(opRight));
					}
				}

				opName = tokens[0];
			}

			auto it = this->opcodes.find(opName);
			if (it == this->opcodes.end()) {
				abort();
			}

			this->pc += it->second(*x, *y);
		}
	}

	virtual int set(Operand &x, const Operand &y) {
		x.val(y.val());

		return 1;
	}

	virtual int sub(Operand &x, const Operand &y) {
		x.val(x.val() - y.val());

		return 1;
	}

	virtual int mul(Operand &x, const Operand &y) {
		x.val(x.val() * y.val());

		return 1;
	}

	virtual int jnz(Operand &x, const Operand &y) {
		if (x.val() != 0) {
			return y.val();
		}

		return 1;
	}
};


class CoprocessorWithStatistics : public Coprocessor {
	public:
		CoprocessorWithStatistics() {
			this->mulCalled = 0;
		}

		int getMulCalled() const {
			return this->mulCalled;
		}

		virtual int mul(Operand &x, const Operand &y) override {
			this->mulCalled++;

			return this->Coprocessor::mul(x, y);
		}

	private:
		int mulCalled;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		CoprocessorWithStatistics c;

		c.load(lines);

		c.run();

		PRINTF(("PART_A: %d", c.getMulCalled()));
	}

	{
		// a = 1

		// 1   set b 79
		// 2   set c b
		// 3   jnz a lab1 (2)
		// 4   jnz 1 lab2 (5) -> always true
		// lab1:
		// 5   mul b 100
		// 6   sub b -100000
		// 7   set c b
		// 8   sub c -17000
		// lab2
		// 9   set f 1
		// 10  set d 2
		// lab5
		// 11  set e 2
		// lab4
		// 12  set g d
		// 13  mul g e
		// 14  sub g b
		// 15  jnz g lab3 (2)
		// 16  set f 0
		// lab3
		// 17  sub e -1
		// 18  set g e
		// 19  sub g b
		// 20  jnz g lab4 (-8)

		// 21  sub d -1
		// 22  set g d
		// 23  sub g b
		// 24  jnz g lab5 (-13)

		// 25  jnz f lab6 (2)
		// 26  sub h -1
		// lab6
		// 27  set g b
		// 28  sub g c
		// 29  jnz g 2
		// 30  jnz 1 3
		// 31  sub b -17
		// 32  jnz 1 -23

		int64_t a = 0;
		int64_t b = 0;
		int64_t c = 0;
		int64_t d = 0;
		int64_t e = 0;
		int64_t f = 0;
		int64_t g = 0;
		int64_t h = 0;

		a = 1;
		b = 79;
		c = b;

		// lab1
		if (a != 0) {
			b *= 100;
			b -= -100000;
			c  = b;
			c -= -17000;
		}

		// lab2
		do {
			f = 1;
			d = 2;

			do {
				// lab4
				e = b;

				if (((e % d) == 0) && ((e - 1) * d) >= b) {
					f = 0;
				}

				g = ++d - b;

			} while (g != 0);

			if (f == 0) {
				h++;
			}

			g  = b;
			g -= c;

			if (g == 0) {
				break;
			}

			b += 17;

		} while (true);

		PRINTF(("PART_B: %"PRId64, h));
	}

	return 0;
}
