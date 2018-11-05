#include <iostream.h>
#include "core.h"
#include "pcb.h"

extern int userMain(int, char*[]);

extern PCB* initialPCB;
extern PCB* idlePCB;

int execCode = 0;

int main(int argc, char* argv[]) {
	PCB::running = initialPCB;
	idlePCB->pid = 1;

	initialize_();
	execCode = userMain(argc, argv);
	restore_();

	return execCode;
}
