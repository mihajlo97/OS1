#ifndef _queue_h_
#define _queue_h_

typedef int ID;

class PCB;

class Queue {

protected:
	volatile PCB* head;
	volatile PCB* tail;

public:
	Queue();
	~Queue();

	unsigned isEmpty();
	void insert(PCB*);
	PCB* dequeue();
	PCB* dequeue(ID);
	void remove(ID);
	void clearQueue();

};

class SleepQueue : public Queue {

public:
	void insert(PCB*);
	void performTick();
	int readyForAwakening();

};

#endif
