#ifndef _sys_ev_h_
#define _sys_ev_h_

typedef unsigned char IVTNo;
typedef void interrupt (*InterruptRoutine)(...);
typedef int ID;

class PCB;

class Queue;

class KernelEv {

public:
	ID ownerID;
	IVTNo entryID;
	InterruptRoutine overridenRoutine;
	Queue* blocked;

	KernelEv(IVTNo);
	~KernelEv();

	void wait();
	void signal();

};

#endif
