#include <stdlib.h>
#include "platform.h"
#include "raylib.h"
#include "../assets/beep.h"

struct Platform {
    RenderTexture2D renderTarget;
    Sound beepSound;
};

Platform* PlatformCreate() {
    return malloc(sizeof(struct Platform));
}

void PlatformDestroy(Platform* platform) {
    free(platform);
}

int PlatformFileExists(const char *fileName) {
    return FileExists(fileName);
}

double PlatformGetTimeMs(void) {
    return GetTime() * 1000.0;
}

unsigned char *PlatformLoadFileData(const char *fileName, int *dataSize) {
    return LoadFileData(fileName, dataSize);
}

void PlatformUnloadFileData(unsigned char *data) {
    UnloadFileData(data);
}

int PlatformIsKeyUp(int key) {
    return IsKeyUp(key);
}

int PlatformWindowShouldClose(void) {
    return WindowShouldClose();
}

void PlatformInitSound(Platform *platform) {
    InitAudioDevice();
    Wave wave = LoadWaveFromMemory(".wav", beep_wav, beep_wav_len); {
        platform->beepSound = LoadSoundFromWave(wave); 
    } UnloadWave(wave);
}

void PlatformPlayBeepSound(Platform *platform) {
    PlaySound(platform->beepSound);
}

void PlatformDestroySound(Platform *platform) {
    UnloadSound(platform->beepSound);
    CloseAudioDevice();
}

void PlatformCreateWindow(Platform *platform, int width, int height, const char *title, int fps) {
    SetTargetFPS(fps);
    InitWindow(width, height, title);
}

void PlatformDestroyWindow(Platform *platform) {
    CloseWindow();
}

void PlatformInitRenderTarget(Platform *platform, int width, int height) {
    platform->renderTarget = LoadRenderTexture(width, height);
    SetTextureFilter(platform->renderTarget.texture, TEXTURE_FILTER_POINT);
}

void PlatformDestroyRenderTarget(Platform *platform) {
    UnloadRenderTexture(platform->renderTarget);
}

void PlatformRenderDisplayBufferToRenderTarget(Platform *platform, uint8_t *buffer, int width, int height) {
    BeginTextureMode(platform->renderTarget);
        ClearBackground(BLACK);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (buffer[y * width + x] != 0) {
                    DrawPixel(x, y, RAYWHITE);  
                }
            }
        }
    EndTextureMode();
}

void PlatformDrawRenderTarget(Platform *platform, int scale) {
    BeginDrawing();
        RenderTexture2D target = platform->renderTarget;

        DrawTexturePro(target.texture, 
            (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height },
            (Rectangle){ 0, 0, (float)target.texture.width * scale, (float)target.texture.height * scale },
            (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}
