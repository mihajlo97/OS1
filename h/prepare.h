#ifndef _prepare_h_
#define _prepare_h_

#include "IVTentry.h"

#define PREPAREENTRY(IVTNum, invokeOriginal)\
	void interrupt intr##IVTNum() {\
		IVTEntry::obtainOriginalRoutine(IVTEntry::callbackRoutine, IVTNum);\
		IVTEntry::triggerSignal(IVTNum);\
		if (invokeOriginal) {\
			IVTEntry::callbackRoutine();\
		}\
	}\
	IVTEntry entry##IVTNo((IVTNo) IVTNum, (InterruptRoutine) intr##IVTNum);

#endif
