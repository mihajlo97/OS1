#include <dos.h>
#include "pcb.h"
#include "core.h"
#include "thread.h"
#include "queue.h"
#include "SCHEDULE.H"

extern Queue* Finished;
extern volatile Queue* internalQueue;

volatile ID PCB::idGen = 2;
volatile unsigned long PCB::activeThreads = 0;

PCB::PCB() {
	asm pushf
	asm cli;
	finished = 0;
	quant = defaultTimeSlice;
	pid = 0;
	semID = SEM_NOT_BLOCKED;
	sleepTimer = 0;

	next = 0;
	stack = 0;
	myThread = 0;
	waitToCompleteQueue = 0;
	asm popf;
}

PCB::PCB(Time allocTime, void (*procedure)(), StackSize stackSize) {
	asm pushf
	asm cli;
	stack = new unsigned[stackSize];
	PCB::createThread(procedure, stackSize);

	finished = 0;
	quant = allocTime;
	sleepTimer = 0;
	pid = PCB::idGen++;
	semID = SEM_NOT_BLOCKED;

	next = 0;
	waitToCompleteQueue = new Queue();
	asm popf;
}

PCB::PCB(StackSize stackSize, Time allocTime, Thread* parentThread) {
	stack = new unsigned[stackSize];
	PCB::createThread(threadRunWrapper, stackSize);

	finished = 0;
	quant = allocTime;
	sleepTimer = 0;
	pid = PCB::idGen++;
	semID = SEM_NOT_BLOCKED;

	myThread = parentThread;
	next = 0;
	waitToCompleteQueue = new Queue();
}

PCB::~PCB() {
	asm pushf
	asm cli;
	Finished->remove(this->pid);
	delete stack;
	delete waitToCompleteQueue;
	asm popf;
}

void PCB::createThread(void (*procedure)(), StackSize stackSize = defaultStackSize) {
#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 1] = 0x200; //push PSW to thread stack (enable interrupt bit)
	stack[stackSize - 2] = FP_SEG(procedure); //push PC of procedure to thread stack
	stack[stackSize - 3] = FP_OFF(procedure);

	//[stackSize - 4]...[stackSize - 11] - push CPU registers to thread stack

	sp = FP_OFF(stack + (stackSize - 12)); //push SP to thread stack
	ss = FP_SEG(stack + (stackSize - 12));
	bp = FP_OFF(stack + (stackSize - 12)); //push BP to thread stack
#endif
}

void PCB::start() {
	asm pushf
	asm cli;
	PCB::activeThreads++;
	insertThread_(this);
	asm popf;
}

void PCB::finalizeThread() {
	asm pushf
	asm cli;
	PCB::activeThreads--;
	finished = 1;
	semID = SEM_NOT_BLOCKED;

	while(waitToCompleteQueue->isEmpty() == 0) {
		insertThread_(waitToCompleteQueue->dequeue());
	}
	asm popf;
}

void PCB::threadRunWrapper() {
	PCB::running->myThread->run();
	((PCB*)PCB::running)->finalizeThread();
	dispatch_();
}

void PCB::waitToComplete() {
	if (!finished) {
		PCB::running->semID = WAIT_TO_COMPLETE_BLOCK;
		internalQueue = waitToCompleteQueue;
		block_();
	}
}

void PCB::sleep(Time timeToSleep) {
	PCB::running->sleepTimer = timeToSleep;
	sleep_();
}

void idleThread() {
	while(PCB::activeThreads);
}
