#ifndef _core_h_
#define _core_h_

//user lock macros
#define SOFT_LOCK softLock_();

#define SOFT_OPEN softOpen_();

class PCB;

enum QueueSelect {
	INTERNAL = -1, //does not get inserted into a global queue, but an internal one
	SCHEDULER = 0, //global ready queue
	SLEEPING = 1, //global sleep queue
	BLOCKED = 2, // global blocked queue
	FINISHED = 3 // global finished queue
};

//scheduler
void insertThread_(PCB*);

PCB* obtainThread_();

//user locks
void softLock_();

void softOpen_();

//system calls
void block_();

void unblock_();

void sleep_();

void dispatch_();

void interrupt timer();

void initialize_();

void restore_();

#endif
