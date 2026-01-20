#include "Backend/MegaDrive.h"

#include "Game.h"

#ifdef SCP_PSP
#include <pspkernel.h>
#include <pspdebug.h>

PSP_MODULE_INFO("SoniCPort", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

int exit_callback(int arg1, int arg2, void *common)
{
	(void)arg1;
	(void)arg2;
	(void)common;
	sceKernelExitGame();
	return 0;
}

int callback_thread(SceSize args, void *argp)
{
	(void)args;
	(void)argp;
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int setup_callbacks(void)
{
	int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0)
		sceKernelStartThread(thid, 0, 0);
	return thid;
}
#endif

//Sonic 1 ROM header
static const MD_Header s1_header = {
	//Vectors
	/* Start of program     */ EntryPoint,
	/* Horizontal interrupt */ HBlank,
	/* Vertical interrupt   */ VBlank,
	
	//Game information
	/* Game title           */ "SONIC THE HEDGEHOG",
};

//MegaDrive entry point
int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	
#ifdef SCP_PSP
	setup_callbacks();
#endif
	
	//Start MegaDrive
	return MegaDrive_Start(&s1_header);
}
