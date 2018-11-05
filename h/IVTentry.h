#ifndef _IVTentry_h_
#define _IVTentry_h_

typedef void interrupt (*InterruptRoutine)(...);
typedef unsigned char IVTNo;

class KernelEv;

class IVTEntry {

public:
	//Interrupt Vector Table
	static KernelEv* eventTable[256];
	static InterruptRoutine routineTable[256];

	//auxiliary global address for calling overridden procedures
	static InterruptRoutine callbackRoutine;

	IVTEntry(IVTNo, InterruptRoutine);

	//auxiliary IVT procedures
	static void obtainOriginalRoutine(InterruptRoutine, IVTNo);
	static void triggerSignal(IVTNo);

};

InterruptRoutine switchRoutines_(IVTNo, InterruptRoutine);

#endif
