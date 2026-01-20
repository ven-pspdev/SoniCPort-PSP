#include "Backend/Joypad.h"

#include <pspctrl.h>

static int exit_requested = 0;

int Input_HandleEvents()
{
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	
	if ((pad.Buttons & (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_START)) == 
	    (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_START))
	{
		exit_requested = 1;
	}
	
	return exit_requested;
}

uint8_t Input_GetState1()
{
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	
	uint8_t result = 0;
	
	if (pad.Buttons & PSP_CTRL_UP)    result |= JPAD_UP;
	if (pad.Buttons & PSP_CTRL_DOWN)  result |= JPAD_DOWN;
	if (pad.Buttons & PSP_CTRL_LEFT)  result |= JPAD_LEFT;
	if (pad.Buttons & PSP_CTRL_RIGHT) result |= JPAD_RIGHT;
	
	if (pad.Buttons & PSP_CTRL_CROSS)    result |= JPAD_A;
	if (pad.Buttons & PSP_CTRL_SQUARE)   result |= JPAD_B;
	if (pad.Buttons & PSP_CTRL_CIRCLE)   result |= JPAD_C;
	if (pad.Buttons & PSP_CTRL_START)    result |= JPAD_START;
	
	if (pad.Buttons & PSP_CTRL_TRIANGLE) result |= JPAD_A;
	
	if (pad.Lx < 64)  result |= JPAD_LEFT;
	if (pad.Lx > 192) result |= JPAD_RIGHT;
	if (pad.Ly < 64)  result |= JPAD_UP;
	if (pad.Ly > 192) result |= JPAD_DOWN;
	
	return result;
}

uint8_t Input_GetState2()
{
	return 0;
}