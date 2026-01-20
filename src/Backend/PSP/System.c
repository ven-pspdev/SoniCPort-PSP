#include "../MegaDrive.h"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psppower.h>

int System_Init(const MD_Header *header)
{
	(void)header;
	
	scePowerSetClockFrequency(333, 333, 166);
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	
	return 0;
}

void System_Quit()
{
	sceKernelExitGame();
}