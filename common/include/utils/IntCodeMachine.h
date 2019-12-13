#ifndef UTILS_INTCODEMACHINE_H_
#define UTILS_INTCODEMACHINE_H_

#include <stdint.h>
#include <vector>
#include <set>


class IntCodeMachine {
	private:
		enum Address {
			ADDRESS_POSITION,
			ADDRESS_IMMEDIATE,
			ADDRESS_RELATIVE,
		};
	public:
		IntCodeMachine(const std::vector<int64_t> &program);
		virtual ~IntCodeMachine();

		bool run();

		void reset();
		bool finished() const;

		std::vector<int64_t> &getMemory();

		std::string getAsm(bool readPositionAddresses);

		void setPc(int64_t pc);

		void addMemoryWatch(int64_t address);

	protected:
		virtual bool onOut(int64_t value);
		virtual bool onIn(int64_t &value);

		virtual void onMemoryWrite(int64_t address, int64_t currentValue, int64_t newValue);
		virtual void onMemoryRead(int64_t address, int64_t currentValue);

	private:
		bool handleOpcode(int code, int64_t arg1, int64_t arg2, int64_t arg3);
		static std::string _opcodeToAsm(const std::vector<int64_t> &program, int pc, int code, int arg1Mode, int64_t arg1, int arg2Mode, int64_t arg2, int arg3Mode, int64_t arg3, int &codeSize, bool readPositionParameters);
		static std::string _addrToAsm(const std::vector<int64_t> &program, int argMode, int64_t arg, bool readPositionParameters);

		void _memWrite(int64_t address, int64_t value);
		int64_t _memRead(int64_t address);

	private:
		bool eop;
		int64_t pc;
		int64_t relativeBase;
		std::vector<int64_t> memory;
		std::vector<int64_t> program;

		std::set<int64_t> watchedAddresses;
};


#endif /* UTILS_INTCODEMACHINE_H_ */
