#include "event.h"
#include "sys_ev.h"

Event::Event(IVTNo entryNum) {
	asm pushf
	asm cli;
	myImpl = new KernelEv(entryNum);
	asm popf;
}

Event::~Event() {
	asm pushf
	asm cli;
	delete myImpl;
	asm popf;
}

void Event::wait() {
	myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}
