#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef struct Platform Platform;

Platform* PlatformCreate(void);
void PlatformDestroy(Platform* platform);

int PlatformFileExists(const char *fileName);
double PlatformGetTimeMs(void); 
unsigned char *PlatformLoadFileData(const char *fileName, int *dataSize);
void PlatformUnloadFileData(unsigned char *data);   
int PlatformIsKeyUp(int key);

void PlatformCreateWindow(Platform *platform, int width, int height, const char *title, int fps);
int PlatformWindowShouldClose(void); 
void PlatformDestroyWindow(Platform *platform);

void PlatformInitSound(Platform *platform);
void PlatformPlayBeepSound(Platform *platform);
void PlatformDestroySound(Platform *platform);

void PlatformInitRenderTarget(Platform *platform, int width, int height);
void PlatformRenderDisplayBufferToRenderTarget(Platform *platform, uint8_t *buffer, int width, int height);
void PlatformDrawRenderTarget(Platform *platform, int scale);
void PlatformDestroyRenderTarget(Platform *platform);

#define PLATFORM_KEY_ONE 49
#define PLATFORM_KEY_TWO 50
#define PLATFORM_KEY_THREE 51
#define PLATFORM_KEY_FOUR 52
#define PLATFORM_KEY_A 65
#define PLATFORM_KEY_C 67
#define PLATFORM_KEY_D 68
#define PLATFORM_KEY_E 69
#define PLATFORM_KEY_F 70
#define PLATFORM_KEY_Q 81
#define PLATFORM_KEY_R 82
#define PLATFORM_KEY_S 83
#define PLATFORM_KEY_V 86
#define PLATFORM_KEY_W 87
#define PLATFORM_KEY_X 88
#define PLATFORM_KEY_Z 90

#endif