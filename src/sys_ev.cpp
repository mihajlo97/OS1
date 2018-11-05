#include "sys_ev.h"
#include "IVTentry.h"
#include "core.h"
#include "pcb.h"
#include "queue.h"

extern volatile Queue* internalQueue;
extern volatile unsigned unblockFromGlobal;

KernelEv::KernelEv(IVTNo entryNum) {
	entryID = entryNum;
	ownerID = PCB::running->pid;
	blocked = new Queue();

	//initialize entry in the table
	IVTEntry::eventTable[entryNum] = this;

	//store overridden routine
	overridenRoutine = (InterruptRoutine)switchRoutines_(entryNum, (InterruptRoutine)IVTEntry::routineTable[entryNum]);
}

KernelEv::~KernelEv() {
	//clear entry from table
	IVTEntry::eventTable[entryID] = 0;

	//restore overriden table
	switchRoutines_(entryID, (InterruptRoutine)overridenRoutine);

	delete blocked;
}

void KernelEv::wait() {
	if (PCB::running->pid == ownerID) {
		asm pushf
		asm cli;
		PCB::running->semID = EVENT_BLOCK;
		internalQueue = this->blocked;
		asm popf;
		block_();
	}
}

void KernelEv::signal() {
	asm pushf
	asm cli;
	unblockFromGlobal = 0;
	internalQueue = this->blocked;
	asm popf;
	unblock_();
}
