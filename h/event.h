#ifndef _event_h_
#define _event_h_

#include "prepare.h"

typedef unsigned char IVTNo;

class KernelEv;

class Event {

public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:
	friend class KernelEv;
	void signal();

private:
	KernelEv* myImpl;

};

#endif
