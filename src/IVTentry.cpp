#include "IVTentry.h"
#include "sys_ev.h"
#include <dos.h>

//Interrupt Vector Table implementation
KernelEv* IVTEntry::eventTable[256];
InterruptRoutine IVTEntry::routineTable[256];

//auxiliary address for calling event overridden interrupt routines
InterruptRoutine IVTEntry::callbackRoutine = 0;

//auxiliary addresses for switching routines
unsigned int newRoutineSEG, newRoutineOFF;
unsigned int oldRoutineSEG, oldRoutineOFF;

InterruptRoutine switchRoutines_(IVTNo entryNum, InterruptRoutine newRoutine)
{
	asm pushf
	asm cli;
	//IVT entry consists of 4 bytes: two lower bytes for offset and two higher for segment
	entryNum *= 4;

	//switch routines and store the overriden one in the global address
#ifndef BCC_BLOCK_IGNORE
	newRoutineSEG = FP_SEG(newRoutine);
	newRoutineOFF = FP_OFF(newRoutine);

	asm {
		push es
		push ax
		push bx

		mov ax, 0
		mov es, ax
		mov bx, word ptr entryNum

		mov ax, word ptr es:bx+2
		mov word ptr oldRoutineSEG, ax
		mov ax, word ptr es:bx
		mov word ptr oldRoutineOFF, ax

		mov ax, word ptr newRoutineSEG
		mov word ptr es:bx+2, ax
		mov ax, word ptr newRoutineOFF
		mov word ptr es:bx, ax

		pop bx
		pop ax
		pop es
	}

	IVTEntry::callbackRoutine = (InterruptRoutine)MK_FP(oldRoutineSEG, oldRoutineOFF);
#endif

	asm popf;
	return IVTEntry::callbackRoutine;
}


IVTEntry::IVTEntry(IVTNo entryID, InterruptRoutine asignedRoutine) {
	asm pushf
	asm cli;
	routineTable[entryID] = asignedRoutine;
	asm popf;
}

void IVTEntry::obtainOriginalRoutine(InterruptRoutine destRoutine, IVTNo src) {
	asm pushf;
	asm cli;
	destRoutine = eventTable[src]->overridenRoutine;
	asm popf;
}

void IVTEntry::triggerSignal(IVTNo eventNum) {
	eventTable[eventNum]->signal();
}
