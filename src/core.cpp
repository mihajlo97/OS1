#include <dos.h>
#include "core.h"
#include "pcb.h"
#include "queue.h"
#include "SCHEDULE.H"

extern void tick();

//synchronous system locks
volatile unsigned syncLock = 1;
volatile int lockCounter = 0;

//timer auxiliary global addresses
unsigned oldTimerOFF, oldTimerSEG;
unsigned tsp, tss, tbp;

//context switching indicators
volatile int timerTick = 10;
volatile unsigned requestContextSwitch = 0;

//system threads
PCB* idlePCB = new PCB(1, idleThread);
PCB* initialPCB = new PCB();
volatile PCB* PCB::running = 0;

//system queues
Queue* Blocked = new Queue();
Queue* Finished = new Queue();
SleepQueue* Sleeping = new SleepQueue();

//queue selection and unblocking
volatile unsigned unblockFromGlobal = 0;
volatile QueueSelect queueSelector = 0;
volatile Queue* internalQueue = 0;
volatile PCB* semUnblocked = 0;

//synchronous lock primitives
void softLock_() {
	asm pushf
	asm cli;
	syncLock = 0;
	lockCounter++;
	asm popf;
}

void softOpen_() {
	asm pushf
	asm cli;
	if (--lockCounter == 0) {
		syncLock = 1;
	}
	if (lockCounter < 0) {
		lockCounter = 0;
	}
	if (requestContextSwitch) {
		dispatch_();
	}
	asm popf;
}

//system scheduling
void insertThread_(PCB* thread) {
	if(thread && thread != idlePCB && thread != initialPCB) {
		thread->next = 0;
		Scheduler::put(thread);
	}
}

PCB* obtainThread_() {
	PCB* thread = Scheduler::get();
	if (thread == 0) {
		if (PCB::activeThreads) {
			thread = idlePCB;
		}
		else {
			thread = initialPCB;
		}
	}
	thread->next = 0;
	return thread;
}

void interrupt timer() {
	//asynchronous hardware interrupt - timer tick
	if (!requestContextSwitch) {
		timerTick--;
		Sleeping->performTick();

		//wake up threads with exceeded sleeping timers
		while(Sleeping->readyForAwakening() > 0) {
			insertThread_(Sleeping->dequeue());
		}
	}
	//synchronous interrupt call - context switching
	if (timerTick == 0 || requestContextSwitch) {
		if (syncLock == 1) {
			//save running context
			asm {
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			PCB::running->sp = tsp;
			PCB::running->ss = tss;
			PCB::running->bp = tbp;

			//select queue to store the running thread
			switch(queueSelector) {
				case SCHEDULER: {
					insertThread_((PCB*)PCB::running);
					break;
				}
				case SLEEPING: {
					Sleeping->insert((PCB*)PCB::running);
					break;
				}
				case BLOCKED: {
					Blocked->insert((PCB*)PCB::running);
					break;
				}
				case FINISHED: {
					Finished->insert((PCB*)PCB::running);
					break;
				}
				case INTERNAL: {
					((Queue*)internalQueue)->insert((PCB*)PCB::running);
				}
			}

			//obtain a thread from the ready queue
			PCB::running = (PCB*)obtainThread_();

			//restore context
			tsp = PCB::running->sp;
			tss = PCB::running->ss;
			tbp = PCB::running->bp;

			//load thread quant into timer
			timerTick = PCB::running->quant;

			//load context into CPU
			asm {
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}

			//reset flags
			requestContextSwitch = 0;
			queueSelector = SCHEDULER;

		}
		else {
			requestContextSwitch = 1;
		}
	}
	//asynchronous hardware interrupt - timer system call
	if (!requestContextSwitch) {
		tick();
		asm int 60h;
	}
}

void dispatch_() {
	asm pushf
	asm cli;
	requestContextSwitch = 1;
	queueSelector = (PCB::running->finished == 0) ? SCHEDULER : FINISHED;
	asm popf;
	asm int 8h;
}

void sleep_() {
	asm pushf
	asm cli;
	requestContextSwitch = 1;
	queueSelector = (PCB::running->finished == 0) ? SLEEPING : FINISHED;
	asm popf;
	asm int 8h;
}

void block_() {
	asm pushf
	asm cli;
	requestContextSwitch = 1;
	if (PCB::running->finished == 0) {
		//if thread does not block on a semaphore, event or waitToComplete, add to global blocked queue
		queueSelector = (PCB::running->semID == SEM_NOT_BLOCKED) ? BLOCKED : INTERNAL;
	}
	else {
		queueSelector = FINISHED;
	}
	asm popf;
	asm int 8h;
}

void unblock_() {
	asm pushf
	asm cli;
	if (unblockFromGlobal) {
		insertThread_(Blocked->dequeue());
	}
	else {
		semUnblocked = ((Queue*)internalQueue)->dequeue();
		semUnblocked->semID = SEM_NOT_BLOCKED;
		insertThread_((PCB*)semUnblocked);
	}
	unblockFromGlobal = 1;
	asm popf;
}

//interrupt routine switching
void initialize_() {
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr es:0022h
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer

		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

void restore_() {
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}

	delete Blocked;
	delete Finished;
	delete Sleeping;
	delete idlePCB;
}
