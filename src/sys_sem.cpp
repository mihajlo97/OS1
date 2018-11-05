#include "sys_sem.h"
#include "core.h"
#include "queue.h"
#include "pcb.h"
#include "SCHEDULE.H"

extern volatile unsigned unblockFromGlobal;
extern volatile Queue* internalQueue;

volatile ID KernelSem::genID = 0;

KernelSem::KernelSem(int initVal) {
	lockVal = initVal;
	semID = ++genID;
	blocked = new Queue();
}

KernelSem::~KernelSem() {
	while (lockVal < 0) {
		this->signal();
	}
	asm pushf
	asm cli;
	delete blocked;
	asm popf;
}

int KernelSem::val() const {
	return lockVal;
}

int KernelSem::wait(int toBlock) {
	asm pushf;
	asm cli;

	int opRes;

	if (toBlock) {
		if (--lockVal < 0) {
			PCB::running->semID = this->semID;
			internalQueue = this->blocked;
			opRes = 1;
			asm popf;
			block_();
		}
		else {
			opRes = 0;
			asm popf;
		}
	}
	else {
		if (lockVal <= 0) {
			opRes = -1;
		}
		else {
			--lockVal;
			opRes = 0;
		}
		asm popf;
	}

	return opRes;
}

void KernelSem::signal() {
	if (lockVal++ < 0) {
		asm pushf;
		asm cli;
		unblockFromGlobal = 0;
		internalQueue = this->blocked;
		asm popf;
		unblock_();
	}
}
