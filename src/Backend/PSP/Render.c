#include "../VDP.h"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>

#include <string.h>

#define PSP_SCREEN_WIDTH  480
#define PSP_SCREEN_HEIGHT 272
#define PSP_LINE_SIZE     512

#define SCALED_WIDTH  388
#define SCALED_HEIGHT 272
#define OFFSET_X      ((PSP_SCREEN_WIDTH - SCALED_WIDTH) / 2)

#define FRAME_SIZE (PSP_LINE_SIZE * PSP_SCREEN_HEIGHT * 4)

static unsigned int __attribute__((aligned(16))) gu_list[262144];
static uint32_t __attribute__((aligned(16))) texture[512 * 256];

typedef struct {
	unsigned short u, v;
	short x, y, z;
} Vertex;

static void *getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
{
	static unsigned int staticOffset = 0;
	unsigned int memSize = width * height * (psm == GU_PSM_8888 ? 4 : 2);
	void *result = (void*)staticOffset;
	staticOffset += memSize;
	return result;
}

int Render_Init(const MD_Header *header)
{
	(void)header;
	
	void *fbp0 = getStaticVramBuffer(PSP_LINE_SIZE, PSP_SCREEN_HEIGHT, GU_PSM_8888);
	void *fbp1 = getStaticVramBuffer(PSP_LINE_SIZE, PSP_SCREEN_HEIGHT, GU_PSM_8888);
	
	sceGuInit();
	sceGuStart(GU_DIRECT, gu_list);
	
	sceGuDrawBuffer(GU_PSM_8888, fbp0, PSP_LINE_SIZE);
	sceGuDispBuffer(PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT, fbp1, PSP_LINE_SIZE);
	
	sceGuOffset(2048 - (PSP_SCREEN_WIDTH / 2), 2048 - (PSP_SCREEN_HEIGHT / 2));
	sceGuViewport(2048, 2048, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT);
	sceGuScissor(0, 0, PSP_SCREEN_WIDTH, PSP_SCREEN_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	
	sceGuDisable(GU_DEPTH_TEST);
	sceGuDisable(GU_CULL_FACE);
	sceGuDisable(GU_LIGHTING);
	sceGuDisable(GU_ALPHA_TEST);
	sceGuDisable(GU_BLEND);
	
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuTexWrap(GU_CLAMP, GU_CLAMP);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	
	sceGuClearColor(0xFF000000);
	
	sceGuFinish();
	sceGuSync(0, 0);
	
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
	
	return 0;
}

void Render_Quit()
{
	sceGuTerm();
}

void Render_Screen(const uint32_t *screen)
{
	uint32_t *dst = texture;
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		memcpy(dst, screen, SCREEN_WIDTH * 4);
		dst += 512;
		screen += SCREEN_WIDTH + (VDP_INTERNAL_PAD * 2);
	}
	
	sceKernelDcacheWritebackInvalidateAll();
	
	sceGuStart(GU_DIRECT, gu_list);
	sceGuClear(GU_COLOR_BUFFER_BIT);
	
	sceGuTexImage(0, 512, 256, 512, texture);
	
	Vertex *vertices = (Vertex*)sceGuGetMemory(2 * sizeof(Vertex));
	
	vertices[0].u = 0;
	vertices[0].v = 0;
	vertices[0].x = OFFSET_X;
	vertices[0].y = 0;
	vertices[0].z = 0;
	
	vertices[1].u = SCREEN_WIDTH;
	vertices[1].v = SCREEN_HEIGHT;
	vertices[1].x = OFFSET_X + SCALED_WIDTH;
	vertices[1].y = SCALED_HEIGHT;
	vertices[1].z = 0;
	
	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
	
	sceGuFinish();
	sceGuSync(0, 0);
	
	//sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}
