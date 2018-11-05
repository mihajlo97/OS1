#ifndef _sys_sem_h_
#define _sys_sem_h_

typedef int ID;

class Queue;

class KernelSem {

public:
	int lockVal;
	ID semID;
	Queue* blocked;

	KernelSem(int initVal = 1);
	~KernelSem();

	int wait(int toBlock = 1);
	void signal();
	int val() const;

	static volatile ID genID;

};

#endif
