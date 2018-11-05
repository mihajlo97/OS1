#ifndef _thread_h_
#define _thread_h_

typedef unsigned long StackSize;
typedef unsigned int Time;
typedef int ID;

const StackSize defaultStackSize = 4096;
const Time defaultTimeSlice = 2;

class PCB;

class Thread {

public:
	void start();
	void waitToComplete();
	virtual ~Thread();

	static void sleep(Time timeToSleep);

protected:
	friend class PCB;
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run() {}

private:
	PCB* myPCB;

};

void dispatch();

#endif
