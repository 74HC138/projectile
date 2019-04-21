
#include <citro2d.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "music0.h"
#include "music1.h"
#include "music2.h"
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
#define SAMPLERATE 	  32728
#define SAMPLESPERBUF (SAMPLERATE / 30)
#define BYTESPERSAMPLE 4

#define TEXT_BLACK			30
#define TEXT_RED			31
#define TEXT_GREEN			32
#define TEXT_YELLOW			33
#define TEXT_BLUE			34
#define TEXT_MAGENTA		35
#define TEXT_CYAN			36
#define TEXT_WHITE			37


//{ structures
	// Simple sprite struct
	typedef struct {
		C2D_Sprite spr;
		float dx, dy; // velocity
	} Sprite;
//} structures
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
	ayemu_ay_t ay;
	int music_offset = 0;
	int music_calls = 0;
	int SAMPLES_PER_FRAME = SAMPLERATE / 60;
	float music_volume = 1;
	float music_volume_old = 1;
	int music_id;
	int rider;
	int menuPos;
	bool music_chip;
	int ym_clock = 1773400;
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
//}
//{ music core
	//---------------------------------------------------------------------------------
	void initPsg() {
	//---------------------------------------------------------------------------------
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
		DSP_FlushDataCache(audioBuffer,size);
	}
	//---------------------------------------------------------------------------------
	void updateReg() {
	//---------------------------------------------------------------------------------
		unsigned char regs[14];
		music_calls++;
		if (music_calls > SAMPLES_PER_FRAME) {
			music_calls = 0;
			switch (music_id) {
				case 0:	
					for (int a = 0; a < 14; a++) {
						regs[a] = music_data0[a][music_offset];
					}
					ayemu_set_regs(&ay, regs);
					music_offset++;
					if (music_offset > sizeof(music_data0[0])/sizeof(music_data0[0][0])) {
						music_offset = 0;
					}
					break;
				case 1:
					for (int a = 0; a < 14; a++) {
						regs[a] = music_data1[a][music_offset];
					}
					ayemu_set_regs(&ay, regs);
					music_offset++;
					if (music_offset > sizeof(music_data1[0])/sizeof(music_data1[0][0])) {
						music_offset = 0;
					}
					break;
				case 2:
					for (int a = 0; a < 14; a++) {
						regs[a] = music_data2[a][music_offset];
					}
					ayemu_set_regs(&ay, regs);
					music_offset++;
					if (music_offset > sizeof(music_data2[0])/sizeof(music_data2[0][0])) {
						music_offset = 0;
					}
					break;
				default:
					for (int a = 0; a < 14; a++) {
						regs[a] = music_data0[a][music_offset];
					}
					ayemu_set_regs(&ay, regs);
					music_offset++;
					if (music_offset > sizeof(music_data0[0])/sizeof(music_data0[0][0])) {
						music_offset = 0;
					}
					break;
			}
			prepare_generation(&ay);
		}
	}
//} music core
//{ audio core
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
	//---------------------------------------------------------------------------------
	void ndspSetVol(int channel, float vol) {
	//---------------------------------------------------------------------------------
		float mix[12];
		mix[0] = vol;
		mix[1] = vol;
		ndspChnSetMix(channel, mix);
	}
//} audio core
//{ sample core
//} sample core
//{ graphics core
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
	void drawCursor(float angle) {
	//---------------------------------------------------------------------------------
		Sprite* cursorSpr = &cursor;
		C2D_SpriteSetRotation(&cursorSpr->spr, angle);
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
//} graphics core
//{ interface core
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
//} interface core
//{ menu core
	//---------------------------------------------------------------------------------
	void drawMenu() {
	//---------------------------------------------------------------------------------
		switch (rider) {
			case 0:
				drawText("Music>\x1b[K", 1);
				drawText("Volume\x1b[K", 2);
				drawText("Samples per frame\x1b[K", 3);
				drawText("clock freq\x1b[K", 4);
				drawText("change music\x1b[K", 5);
				drawText("switch chip type\x1b[K", 6);
				printf("\x1b[2;30H%.2f\x1b[K", music_volume);
				printf("\x1b[3;30H%i\x1b[K", SAMPLES_PER_FRAME);
				printf("\x1b[4;30H%i\x1b[K", ym_clock);
				printf("\x1b[6;30H%s\x1b[K", music_chip ? "ym":"ay"); 
				break;
			case 1:
				drawText("<Graphics>\x1b[K", 1);
				drawText("Sprites\x1b[K", 2);
				drawText("CPU:\x1b[K", 3);
				drawText("GPU:\x1b[K", 4);
				drawText("CmdBuf:\x1b[K", 5);
				printf("\x1b[2;30H%i\x1b[K", numSprites);
				printf("\x1b[3;30H%6.2f", C3D_GetProcessingTime()*6.0f);
				printf("\x1b[4;30H%6.2f", C3D_GetDrawingTime()*6.0f);
				printf("\x1b[5;30H%6.2f\x1b[K", C3D_GetCmdBufUsage()*100.0f);
				break;
			case 2:
				drawText("<Sound queue\x1b[K", 1);
				drawText("queue sound\x1b[K", 2);
				break;
			default:
				drawText("Music\x1b[K", 1);
				drawText("Volume\x1b[K", 2);
				drawText("Samples per frame\x1b[K", 3);
				drawText("clock freq\x1b[K", 4);
				drawText("change music\x1b[K", 5);
				drawText("switch chip type\x1b[K", 6);
				printf("\x1b[2;30H%.2f\x1b[K", music_volume);
				printf("\x1b[3;30H%i\x1b[K", SAMPLES_PER_FRAME);
				printf("\x1b[4;30H%i\x1b[K", ym_clock);
				printf("\x1b[6;30H%i\x1b[K", music_chip ? "ym":"ay"); 
				break;
		}
	}
	//---------------------------------------------------------------------------------
	void menuMoveUp() {
	//---------------------------------------------------------------------------------
		menuPos--;
		if (menuPos < 1) menuPos = 1;
	}
	//---------------------------------------------------------------------------------
	void menuMoveDown() {
	//---------------------------------------------------------------------------------
		menuPos++;
		switch (rider) {
			case 0:
				if (menuPos > 6) menuPos = 6;
				break;
			case 1:
				if (menuPos > 1) menuPos = 2;
				break;
			case 6:
				if (menuPos > 2) menuPos = 2;
				break;
			default:
				if (menuPos > 6) menuPos = 6;
				break;
		}
	}
	//---------------------------------------------------------------------------------
	void menuMoveLeft() {
	//---------------------------------------------------------------------------------
		if (menuPos == 1) {
			rider--;
			if (rider < 0) rider = 0;
			consoleClear();
		}
		if (rider == 0) {
			switch (menuPos) {
				case 5:
					music_id++;
					if (music_id > 2) music_id = 0;
					music_offset = 0;
					music_calls = 0;
					break;
				case 6:
					music_chip = !music_chip;
					break;
			}
		}
				
	}
	//---------------------------------------------------------------------------------
	void menuMoveRight() {
	//---------------------------------------------------------------------------------
		if (menuPos == 1) {
			rider++;
			if (rider > 2) rider = 2;
			consoleClear();
		}
		if (rider == 0) {
			switch (menuPos) {
				case 5:
					music_id++;
					if (music_id > 2) music_id = 0;
					music_offset = 0;
					music_calls = 0;
					break;
				case 6:
					music_chip = !music_chip;
			}
		}
	}
	//---------------------------------------------------------------------------------
	void menuAddLeft() {
	//---------------------------------------------------------------------------------
		if (rider == 0) {
			switch (menuPos) {
				case 2:
					music_volume = music_volume-0.01;
					if (music_volume < 0) music_volume = 0;
					break;
				case 3:
					SAMPLES_PER_FRAME--;
					if (SAMPLES_PER_FRAME < 1) SAMPLES_PER_FRAME = 1;
					break;
				case 4:
					ym_clock = ym_clock-1000;
					if (ym_clock < 1) ym_clock = 1;
					ayemu_set_chip_freq(&ay, ym_clock);
					break;
			}
		}
		if (rider == 1){
			switch (menuPos) {
				case 2:
					if (numSprites > 1) {
						numSprites--;
					}
					break;
			}
		}
	}
	//---------------------------------------------------------------------------------
	void menuAddRight() {
	//---------------------------------------------------------------------------------
		if (rider == 0) {
			switch (menuPos) {
				case 2:
					music_volume = music_volume+0.01;
					break;
				case 3:
					SAMPLES_PER_FRAME++;
					break;
				case 4:
					ym_clock = ym_clock+1000;
					ayemu_set_chip_freq(&ay, ym_clock);
					break;
			}
		}
		if (rider == 1){
			switch (menuPos) {
				case 2:
					if (numSprites < MAX_SPRITES) {
						numSprites++;
					}
					break;
			}
		}
	}
	//---------------------------------------------------------------------------------
	void drawText(char *text, int y) {
	//---------------------------------------------------------------------------------
		if (y == menuPos) {
			setColor(TEXT_YELLOW);
		} else {
			setColor(TEXT_WHITE);
		}
		printf("\x1b[%i;1H%s%s", y, y == menuPos ? " " : "", text);
		setColor(TEXT_WHITE);
	}
	//---------------------------------------------------------------------------------
	void setColor(uint8_t color) {
	//---------------------------------------------------------------------------------
		printf("\x1b[%im", color);
	}
//}

//---------------------------------------------------------------------------------
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
		if (kDown & KEY_START) break; // break in order to return to hbmenu
		if (kDown & KEY_SELECT) {
			music_id++;
			if (music_id > 2) music_id = 0;
			music_offset = 0;
			music_calls = 0;
		}
		if (kDown & KEY_B) {
			//pause or play 
			bool status = ndspChnIsPaused(0);
			ndspChnSetPaused(0, !status);
		}
		if (kDown & KEY_DUP) menuMoveUp();
		if (kDown & KEY_DDOWN) menuMoveDown();
		if (kDown & KEY_DLEFT) menuMoveLeft();
		if (kDown & KEY_DRIGHT) menuMoveRight();
		
		u32 kHeld = hidKeysHeld();
		if (kHeld & KEY_DRIGHT) menuAddRight();
		if (kHeld & KEY_DLEFT) menuAddLeft();
		/*
		if ((kHeld & KEY_DUP) && numSprites < MAX_SPRITES)
			numSprites++;
		if ((kHeld & KEY_DDOWN) && numSprites > 1)
			numSprites--;
		*/
		if (kHeld & KEY_A) {
			C2D_SpriteSheetFree(spriteSheet);
			C2D_SpriteSheetFree(blockSheet);
			spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
			if (!spriteSheet) svcBreak(USERBREAK_PANIC);
			blockSheet = C2D_SpriteSheetLoad("romfs:/gfx/blocks.t3x");
			if (!blockSheet) svcBreak(USERBREAK_PANIC);
			initSprites();
			loadBlocks();
			initPsg();
		}
		if (music_volume != music_volume_old) {
			ndspSetVol(0, music_volume);
		}
		if (kHeld & KEY_L) moveLeft();
		if (kHeld & KEY_R) moveRight();
		/*
		if (kHeld & KEY_DRIGHT) {
			SAMPLES_PER_FRAME--;
			if (SAMPLES_PER_FRAME <= 1) SAMPLES_PER_FRAME = 1;
		}
		if (kHeld & KEY_DLEFT) {
			SAMPLES_PER_FRAME++;
		}
		*/
		

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		moveSprites();
		drawCursor(calcPadAngle());
		drawMenu();
		drawMap();
		for (size_t i = 0; i < numSprites; i ++)
			C2D_DrawSprite(&sprites[i].spr);
		C2D_DrawSprite(&cursor.spr);
		
		//moved in frame draw to monitore CPU usage
		//likely temporary
		if (waveBuf[fillBlock].status == NDSP_WBUF_DONE) {
			fill_buffer(waveBuf[fillBlock].data_pcm16, waveBuf[fillBlock].nsamples, 0);
			ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);
			fillBlock = !fillBlock;
		}
		C3D_FrameEnd(0);
		music_volume_old = music_volume;
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

