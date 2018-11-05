#include "semaphor.h"
#include "sys_sem.h"

Semaphore::Semaphore(int init) {
	asm pushf
	asm cli;
	myImpl = new KernelSem(init);
	asm popf;
}

Semaphore::~Semaphore() {
	delete myImpl;
}

int Semaphore::wait(int toBlock) {
	int res = myImpl->wait(toBlock);
	return res;
}
void Semaphore::signal() {
	myImpl->signal();
}

int Semaphore::val() const {
	return myImpl->val();
}
