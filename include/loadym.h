
#ifndef _LOADYM_H_
#define _LOADYM_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	unsigned char* loadFile(const char* path);
	unsigned char* loadYM(const char* path);
	int getFrames(const char* path);
	unsigned char* getFrame(unsigned char* data, int frame, int numFrames);
	void unloadYM(unsigned char* data);
	
#ifdef __cplusplus
}
#endif

#endif