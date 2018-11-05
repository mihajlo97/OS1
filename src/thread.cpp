#include "thread.h"
#include "pcb.h"
#include "core.h"

Thread::Thread(StackSize stackSize, Time timeSlice) {
	asm pushf
	asm cli;
	if (stackSize > maxStackSize) {
		stackSize = maxStackSize;
	}
	else if (stackSize < minStackSize) {
		stackSize = minStackSize;
	}
	myPCB = new PCB(stackSize, timeSlice, this);
	asm popf;
}

void Thread::start() {
	myPCB->start();
}

void Thread::sleep(Time timeToSleep) {
	PCB::sleep(timeToSleep);
}

void Thread::waitToComplete() {
	myPCB->waitToComplete();
}

Thread::~Thread() {
	this->waitToComplete();
	delete myPCB;
}

void dispatch() {
	dispatch_();
}


