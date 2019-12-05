#ifndef UTILS_INTCODEMACHINE_H_
#define UTILS_INTCODEMACHINE_H_

#include <vector>


class IntCodeMachine {
	public:
		IntCodeMachine();
		virtual ~IntCodeMachine();

		void run(std::vector<int> &program);

	protected:
		virtual void onOut(int value);
		virtual int  onIn();

	private:
		void handleOpcode(std::vector<int> &mem, int code, int arg1, int arg2, int arg3);

	private:
		int pc;
};


#endif /* UTILS_INTCODEMACHINE_H_ */
