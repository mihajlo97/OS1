#ifndef _pcb_h_
#define _pcb_h_

typedef unsigned long StackSize;
typedef unsigned int Time;
typedef int ID;

const StackSize maxStackSize = 64000;
const StackSize minStackSize = 16;

class Thread;

class Queue;

class PCB {

public:
	unsigned ss;
	unsigned sp;
	unsigned bp;
	unsigned finished;
	ID pid;
	ID semID;

	volatile unsigned int quant;
	volatile unsigned long sleepTimer;

	unsigned* stack;
	volatile PCB* next;
	Thread* myThread;
	Queue* waitToCompleteQueue;

	PCB(StackSize, Time, Thread*);
	PCB(Time, void (*procedure)(), StackSize stackSize = minStackSize);
	PCB();
	~PCB();

	void start();
	void waitToComplete();
	static void sleep(Time);

	void createThread(void (*procedure)(), StackSize stackSize);
	void finalizeThread();
	static void threadRunWrapper();

	static volatile unsigned long activeThreads;
	static volatile PCB* running;
	static volatile ID idGen;

};

enum SemBlockFlag {
		SEM_NOT_BLOCKED = 0,
		WAIT_TO_COMPLETE_BLOCK = -1,
		EVENT_BLOCK = -2
		// >0 : blocked on semaphore with its corresponding ID
		// =0 : thread is either unblocked or blocked on the global queue
};

void idleThread();

#endif
