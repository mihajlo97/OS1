#include "queue.h"
#include "core.h"
#include "pcb.h"

Queue::Queue() {
	head = 0;
	tail = 0;
}

void Queue::insert(PCB* thread) {
	if (head == 0) {
		head = tail = thread;
	}
	else {
		tail->next = thread;
		tail = tail->next;
	}
}

PCB* Queue::dequeue() {
	if (head == 0) {
		return 0;
	}

	volatile PCB* thread;
	thread = head;
	head = head->next;
	thread->next = 0;

	return (PCB*)thread;
}

PCB* Queue::dequeue(ID threadID) {
	if (head == 0) {
		return 0;
	}

	volatile PCB* temp = head;
	volatile PCB* aux;

	if (head->pid == threadID) {
		head = head->next;
		temp->next = 0;
		return (PCB*)temp;
	}
	else {
		while(temp->next && temp->next != tail) {
			if (temp->next->pid == threadID) {
				aux = temp->next;
				temp->next = aux->next;
				aux->next = 0;
				return (PCB*)aux;
			}
			temp = temp->next;
		}
		if (temp->next == tail && tail->pid == threadID) {
			aux = tail;
			tail = temp;
			tail->next = 0;
			return (PCB*)aux;
		}
	}

	return 0;
}

void Queue::remove(ID threadID) {
	this->dequeue(threadID);
}

unsigned Queue::isEmpty() {
	return head == 0;
}

void Queue::clearQueue() {
	while(head) {
		tail = head;
		head = head->next;
		delete tail;
	}
	head = tail = 0;
}

Queue::~Queue() {
	this->clearQueue();
}

void SleepQueue::insert(PCB* thread) {
	if (head == 0) {
		head = tail = thread;
		return;
	}

	if (head->sleepTimer > thread->sleepTimer) {
		thread->next = head;
		head = thread;
	}
	else {
		volatile PCB* temp = head;
		while(temp->next) {
			if (temp->next->sleepTimer > thread->sleepTimer) {
				thread->next = temp->next;
				temp->next = thread;
				break;
			}
			temp = temp->next;
		}
		if (temp->next == 0) {
			temp->next = thread;
			tail = thread;
		}
	}
}

void SleepQueue::performTick() {
	volatile PCB* aux = head;
	while(aux) {
		aux->sleepTimer--;
		aux = aux->next;
	}
}

int SleepQueue::readyForAwakening() {
	if (head == 0) {
		return -1;
	}
	return head->sleepTimer == 0;
}
