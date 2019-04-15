#include "emu2149.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <3ds.h>
#include <stdlib.h>

unsigned char* loadFile(const char* path) {
	FILE* f = fopen(path, "rb");
	if (f) {
		fseek(f, 0L, SEEK_END);
		int s = ftell(f);
		fseek(f, 0L, SEEK_SET);
		unsigned char *dat = (char *)malloc((s+1) * sizeof(char));
		if (dat == NULL) {
			return NULL;
		}
		fread(dat, s, 1, f);
		fclose(f);
		return *dat;
	} else {
		return NULL;
	}
}
unsigned char* loadYM(const char* path) {
	unsigned char* data = loadFile(path);
	if (data == NULL) {
		return NULL;
	}
	if (data[0] == 'Y' && data[1] == 'M' && data[2] == '3' && data[3] == '!') {        //checksum 'YM3!' at start of file == YM file
		return *data;
	} else {
		return NULL;
	}
}
int getFrames(const char* path) {
	FILE* f = fopen(path, "rb");
	if (f) {
		fseek(f, 0L, SEEK_END);
		int s = ftell(f);
		fclose(f);
		return (s-4)/14;
	} else {
		return NULL;
	}
}
unsigned char* getFrame(unsigned char* data, int frame, int numFrames) {
	if (frame > numFrames) {
		return NULL;
	}
	char* ret[14];
	for (int i = 0; i < 14; i++) {
		int length = numFrames * 14;
		int index = 4 + ((length/14)*i) + frame;
		ret[i] = data[index];
	}
	return *ret;
}
void unloadYM(unsigned char* data) {
	free(data);
}
		