// Simple citro2d sprite drawing example
// Images borrowed from:
//   https://kenney.nl/assets/space-shooter-redux
#include <citro2d.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//ym2149 lib by digital-sound-antiques (https://github.com/digital-sound-antiques/emu2149)
#include "emu2149.h"
#include "music.h"
#include "ayemu.h"

#define MAX_SPRITES   1000
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define BOTTOM_WIDTH  320
#define BOTTOM_HEIGHT 240
#define MAX_BLOCKS	  256
#define threshhold 	  50
#define blockSize	  16
#define moveSpeed	  2.0f
#define MAP_SX		  40
#define MAP_SY		  40
#define ym_clock	  2000000
#define SAMPLERATE 	  44100
#define SAMPLESPERBUF (SAMPLERATE / 15)
#define BYTESPERSAMPLE 4



// Simple sprite struct
typedef struct {
	C2D_Sprite spr;
	float dx, dy; // velocity
} Sprite;

//{ variables
static C2D_SpriteSheet spriteSheet;
static C2D_SpriteSheet cursorSheet;
static C2D_SpriteSheet blockSheet;
static Sprite sprites[MAX_SPRITES];
static size_t numSprites = MAX_SPRITES/2;
static Sprite cursor;
static C2D_Sprite blocks[MAX_BLOCKS];
float offsetX = 0;
int blockId;
bool pressed = false;
int mapOffsetX;
int mapOffsetY;
//PSG* ym;
ayemu_ay_t ay;
int music_offset = 0;
int music_calls = 0;
int SAMPLES_PER_FRAME = 882;
//}

unsigned char mapData[40][40] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
	{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
	{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}
};

//---------------------------------------------------------------------------------
static void initSprites() {
//---------------------------------------------------------------------------------
	size_t numImages = C2D_SpriteSheetCount(spriteSheet);
	srand(time(NULL));

	for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite* sprite = &sprites[i];

		// Random image, position, rotation and speed
		C2D_SpriteFromSheet(&sprite->spr, spriteSheet, rand() % numImages);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		C2D_SpriteSetRotation(&sprite->spr, C3D_Angle(rand()/(float)RAND_MAX));
		sprite->dx = rand()*4.0f/RAND_MAX - 2.0f;
		sprite->dy = rand()*4.0f/RAND_MAX - 2.0f;
	}
	// cursor
	Sprite* cursorSpr = &cursor;
	C2D_SpriteFromSheet(&cursorSpr->spr, cursorSheet, 1);
	C2D_SpriteSetCenter(&cursorSpr->spr, 1, 1);
	C2D_SpriteSetPos(&cursorSpr->spr, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
}
//---------------------------------------------------------------------------------
static void updateBlockId() {
//---------------------------------------------------------------------------------
	u32 kDown = hidKeysDown();
	if (kDown & KEY_Y) {
		if (!pressed) {
			blockId++;
			if (blockId > C2D_SpriteSheetCount(blockSheet)-1) {
				blockId = C2D_SpriteSheetCount(blockSheet)-1;
			}
		}
		pressed = true;
	} else {
		if (kDown & KEY_X) {
			if (!pressed) {
				blockId--;
				if (blockId < 0) {
					blockId = 0;
				}
			}
			pressed = true;
		} else {
			pressed = false;
		}
	}
}
//---------------------------------------------------------------------------------
static void loadBlocks() {
//---------------------------------------------------------------------------------
	size_t numImages = C2D_SpriteSheetCount(blockSheet);
	if (numImages > MAX_BLOCKS) {
		//throwError("To many blocks to load (out of memory");
		while (aptMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown();
			if (kDown & KEY_START) break;
		}
	}
	for (int i = 0; i < numImages; i++) {
		C2D_SpriteFromSheet(&blocks[i], blockSheet, i);
		C2D_SpriteSetCenter(&blocks[i], 1.0f, 1.0f);
	}
}
//---------------------------------------------------------------------------------
static void moveSprites() {
//---------------------------------------------------------------------------------
	for (size_t i = 0; i < numSprites; i++)
	{
		Sprite* sprite = &sprites[i];
		C2D_SpriteMove(&sprite->spr, sprite->dx, sprite->dy);
		C2D_SpriteRotateDegrees(&sprite->spr, 1.0f);

		// Check for collision with the screen boundaries
		if ((sprite->spr.params.pos.x < sprite->spr.params.pos.w / 2.0f && sprite->dx < 0.0f) ||
			(sprite->spr.params.pos.x > (SCREEN_WIDTH-(sprite->spr.params.pos.w / 2.0f)) && sprite->dx > 0.0f))
			sprite->dx = -sprite->dx;

		if ((sprite->spr.params.pos.y < sprite->spr.params.pos.h / 2.0f && sprite->dy < 0.0f) ||
			(sprite->spr.params.pos.y > (SCREEN_HEIGHT-(sprite->spr.params.pos.h / 2.0f)) && sprite->dy > 0.0f))
			sprite->dy = -sprite->dy;
	}
}
//---------------------------------------------------------------------------------
static void drawMap() {
//---------------------------------------------------------------------------------
	int sx = ceil(SCREEN_WIDTH/blockSize)+2;
	int sy = ceil(SCREEN_HEIGHT/blockSize)+2;
	//int sx = 10;
	//int sy = 10;
	for (int x = -1; x < sx; x++) {
		for (int y = -1; y < sy; y++) {
			//C2D_DrawImageAt(blocks[1], (float) x, (float) y, 0.0f, NULL, 1.0f, 1.0f);
			if (y+1+mapOffsetY >= MAP_SY || y+1+mapOffsetY < 0 || x+1+mapOffsetX >= MAP_SX || x+1+mapOffsetX < 0) {
				blocks[0].params.pos.x = x*blockSize + offsetX;
				blocks[0].params.pos.y = y*blockSize;
				C2D_DrawSprite(&blocks[0]);
			} else {
				blocks[mapData[y+1+mapOffsetY][x+1+mapOffsetX]].params.pos.x = x*blockSize + offsetX;
				blocks[mapData[y+1+mapOffsetY][x+1+mapOffsetX]].params.pos.y = y*blockSize;
				C2D_DrawSprite(&blocks[mapData[y+1+mapOffsetY][x+1+mapOffsetX]]);
			}
		}
	}
}
//---------------------------------------------------------------------------------
static float calcPadAngle() {
//---------------------------------------------------------------------------------
	circlePosition pos;
	hidCircleRead(&pos);
	if (pos.dx > threshhold || pos.dx < -threshhold || pos.dy > threshhold || pos.dy < -threshhold) {
		return atan2(pos.dx,pos.dy)+(M_PI/2.0f);
	}
	return 0.0f;
}
//---------------------------------------------------------------------------------
void drawCursor(float angle) {
//---------------------------------------------------------------------------------
	Sprite* cursorSpr = &cursor;
	C2D_SpriteSetRotation(&cursorSpr->spr, angle);
}	
//---------------------------------------------------------------------------------
void throwError(char *error) {
//---------------------------------------------------------------------------------
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	printf("An error has acourd");
	printf(error);
	printf("Press START to exit");
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) break;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		C3D_FrameEnd(0);
	}
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
}
//---------------------------------------------------------------------------------
void moveRight() {
//---------------------------------------------------------------------------------
	offsetX = offsetX - moveSpeed;
	if (offsetX <= -blockSize) {
		offsetX = offsetX + blockSize;
		mapOffsetX++;
	}
}
//---------------------------------------------------------------------------------
void moveLeft() {
//---------------------------------------------------------------------------------
	offsetX = offsetX + moveSpeed;
	if (offsetX >= blockSize) {
		offsetX = offsetX - blockSize;
		mapOffsetX--;
	}
}
//---------------------------------------------------------------------------------
void drawMenu(int menuPos) {
//---------------------------------------------------------------------------------
	printf("\x1b[9;1HPress Up to increment sprites");
	printf("\x1b[10;1HPress Down to decrement sprites");
	printf("\x1b[11;1HPress L to move Left");
	printf("\x1b[12;1HPress R to move Right");
	printf("\x1b[1;1HSprites: %zu/%u\x1b[K", numSprites, MAX_SPRITES);
	printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
	printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
	printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);
	printf("\x1b[5;1HSamples per frame: %i\x1b[K", SAMPLES_PER_FRAME);
	printf("\x1b[14;1Hmusic offset %i", music_offset);
}
//---------------------------------------------------------------------------------
void initPsg() {
//---------------------------------------------------------------------------------
	//ym = PSG_new(ym_clock, SAMPLERATE);
	//PSG_reset(ym);
	//PSG_set_quality(ym, 2);
	ayemu_init(&ay);
	ayemu_set_chip_freq(&ay, ym_clock);
	int chip = AYEMU_AY;
	ayemu_set_stereo(&ay, AYEMU_MONO, 0);
	ayemu_set_chip_type(&ay, AYEMU_AY_KAY, 0);
	ayemu_set_sound_format(&ay, SAMPLERATE, 1, 16);
	ayemu_reset(&ay);
}
//---------------------------------------------------------------------------------
void fill_buffer(void *audioBuffer, size_t size, size_t offset) {
//---------------------------------------------------------------------------------
	u32 *sound_buf = (u32*)audioBuffer;
	ayemu_gen_sound(&ay, sound_buf, size);
	
	/*
	while (remaining > SAMPLES_PER_FRAME) {
		ayemu_gen_sound(&ay, dest+(size-remaining), SAMPLES_PER_FRAME);
		for (int i = 0; i < 14; i++) {
			regs[i] = music_data[i][music_offset];
		}
		ayemu_set_regs(&ay, regs);
		music_offset++;
		remaining = remaining - SAMPLES_PER_FRAME;
	}
	
	if (remaining > 0) {
		ayemu_gen_sound(&ay, dest+(size-remaining), remaining);
		for (int i = 0; i < 14; i++) {
			regs[i] = music_data[i][music_offset];
		}
		ayemu_set_regs(&ay, regs);
		music_offset++;
	}
	*/
	DSP_FlushDataCache(audioBuffer,size);
	
}
//---------------------------------------------------------------------------------
void initNdsp() {
//---------------------------------------------------------------------------------
	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
	ndspChnSetRate(0, SAMPLERATE);
	ndspChnSetFormat(0, NDSP_FORMAT_MONO_PCM16);
	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0;
	mix[1] = 1.0;
	ndspChnSetMix(0, mix);
}

int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	// Init libs
	romfsInit();
	initPsg();
	ndspWaveBuf waveBuf[2];
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);
	u32 *audioBuffer = (u32*)linearAlloc(SAMPLESPERBUF*BYTESPERSAMPLE*2);
	bool fillBlock = false;
	initNdsp();
	memset(waveBuf,0,sizeof(waveBuf));
	waveBuf[0].data_vaddr = &audioBuffer[0];
	waveBuf[0].nsamples = SAMPLESPERBUF;
	waveBuf[1].data_vaddr = &audioBuffer[SAMPLESPERBUF];
	waveBuf[1].nsamples = SAMPLESPERBUF;
	fill_buffer(audioBuffer, SAMPLESPERBUF * 2, 0);
	size_t stream_offset = SAMPLESPERBUF;
	ndspChnWaveBufAdd(0, &waveBuf[0]);
	ndspChnWaveBufAdd(0, &waveBuf[1]);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) {
		throwError("Error with sprite file");
		return 0;
	}
	cursorSheet = C2D_SpriteSheetLoad("romfs:/gfx/cursor.t3x");
	if (!cursorSheet) {
		throwError("Error with cursor file");
		return 0;
	}

	// Initialize sprites
	initSprites();
	
	blockSheet = C2D_SpriteSheetLoad("romfs:/gfx/blocks.t3x");
	if (!blockSheet) {
		throwError("Error with block sheet");
		return 0;
	}
	loadBlocks();

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		u32 kHeld = hidKeysHeld();
		if ((kHeld & KEY_DUP) && numSprites < MAX_SPRITES)
			numSprites++;
		if ((kHeld & KEY_DDOWN) && numSprites > 1)
			numSprites--;
		if (kHeld & KEY_A) {
			C2D_SpriteSheetFree(spriteSheet);
			C2D_SpriteSheetFree(blockSheet);
			spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
			if (!spriteSheet) svcBreak(USERBREAK_PANIC);
			blockSheet = C2D_SpriteSheetLoad("romfs:/gfx/blocks.t3x");
			if (!blockSheet) svcBreak(USERBREAK_PANIC);
			initSprites();
			loadBlocks();
		}
		if (kHeld & KEY_L) moveLeft();
		if (kHeld & KEY_R) moveRight();
		if (kHeld & KEY_DRIGHT) {
			SAMPLES_PER_FRAME--;
			if (SAMPLES_PER_FRAME <= 1) SAMPLES_PER_FRAME = 1;
		}
		if (kHeld & KEY_DLEFT) {
			SAMPLES_PER_FRAME++;
		}
		
		moveSprites();
		drawCursor(calcPadAngle());
		updateBlockId();
		drawMenu(0);
		if (waveBuf[fillBlock].status == NDSP_WBUF_DONE) {
			fill_buffer(waveBuf[fillBlock].data_pcm16, waveBuf[fillBlock].nsamples, 0);
			ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);
			fillBlock = !fillBlock;
		}

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		drawMap();
		for (size_t i = 0; i < numSprites; i ++)
			C2D_DrawSprite(&sprites[i].spr);
		C2D_DrawSprite(&cursor.spr);
		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);
	ndspExit();
	linearFree(audioBuffer);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}


void updateReg() {
  unsigned char regs[14];
  music_calls++;
  if (music_calls > SAMPLES_PER_FRAME) {
	music_calls = 0;
	for (int a = 0; a < 14; a++) {
	  regs[a] = music_data[a][music_offset];
	}
	ayemu_set_regs(&ay, regs);
	music_offset++;
	if (music_offset > sizeof(music_data[0])/sizeof(music_data[0][0])) {
		music_offset = 0;
	}
	prepare_generation(&ay);
  }
}
