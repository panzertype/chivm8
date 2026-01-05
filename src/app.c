#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "chip8.h"
#include "raylib.h"
#include "../assets/beep.h"

#define SCALE 8
#define FPS CHIP8_DISPLAY_REFRESH_RATE_HZ
#define INSTRUCTIONS_PER_SECOND 800
#define INSTRUCTIONS_PER_FRAME (INSTRUCTIONS_PER_SECOND / FPS)
#define GET_KEY_STATE(key) IsKeyUp(key) ? CHIP8_KEY_UP : CHIP8_KEY_DOWN

typedef struct {
    Chip8 chip8;
    char* romFilePath;
    
    // todo: move to platform layer
    RenderTexture2D renderTarget;
    Sound beepSound;
} App;

void AppInit(App *app) {
    srand(time(NULL));

    memset(app, 0, sizeof(App));
}

void AppHandleCliArgs(App *app, int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: chivm8 <rom file>\n");
        exit(1);
    }

    app->romFilePath = argv[1]; // Safe
    if (!FileExists(app->romFilePath)) {
        fprintf(stderr, "[ERROR] File \"%s\" could not be found\n", argv[1]);
        exit(1);
    };
}

void AppLoadRomIntoChip8(App *app) {
    assert(app->romFilePath != NULL);

    int bytesRead = 0;
    unsigned char *fileData = LoadFileData(app->romFilePath, &bytesRead); {
        if (fileData == NULL) {
            fprintf(stderr, "[ERROR] File could not be read\n");
            exit(1);
        }

        Chip8Init(&app->chip8, fileData, bytesRead);
    } UnloadFileData(fileData);
}

void AppCreateWindow(App *app) {
    SetTargetFPS(FPS);
    InitWindow(CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, "Chivm8");
    InitAudioDevice();

    app->renderTarget = LoadRenderTexture(CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
    SetTextureFilter(app->renderTarget.texture, TEXTURE_FILTER_POINT);

    Wave wave = LoadWaveFromMemory(".wav", beep_wav, beep_wav_len); {
        app->beepSound = LoadSoundFromWave(wave); 
    } UnloadWave(wave);
}

void AppProcessChip8Inputs(App *app) {
    Chip8UpdateKey(&app->chip8, 0x1, GET_KEY_STATE(KEY_ONE));
    Chip8UpdateKey(&app->chip8, 0x2, GET_KEY_STATE(KEY_TWO));
    Chip8UpdateKey(&app->chip8, 0x3, GET_KEY_STATE(KEY_THREE));
    Chip8UpdateKey(&app->chip8, 0xC, GET_KEY_STATE(KEY_FOUR));

    Chip8UpdateKey(&app->chip8, 0x4, GET_KEY_STATE(KEY_Q));
    Chip8UpdateKey(&app->chip8, 0x5, GET_KEY_STATE(KEY_W));
    Chip8UpdateKey(&app->chip8, 0x6, GET_KEY_STATE(KEY_E));
    Chip8UpdateKey(&app->chip8, 0xD, GET_KEY_STATE(KEY_R));

    Chip8UpdateKey(&app->chip8, 0x7, GET_KEY_STATE(KEY_A));
    Chip8UpdateKey(&app->chip8, 0x8, GET_KEY_STATE(KEY_S));
    Chip8UpdateKey(&app->chip8, 0x9, GET_KEY_STATE(KEY_D));
    Chip8UpdateKey(&app->chip8, 0xE, GET_KEY_STATE(KEY_F));

    Chip8UpdateKey(&app->chip8, 0xA, GET_KEY_STATE(KEY_Z));
    Chip8UpdateKey(&app->chip8, 0x0, GET_KEY_STATE(KEY_X));
    Chip8UpdateKey(&app->chip8, 0xB, GET_KEY_STATE(KEY_C));
    Chip8UpdateKey(&app->chip8, 0xF, GET_KEY_STATE(KEY_V));
}

void AppRunChip8(App *app) {
    for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
        Chip8RunCycle(&app->chip8);

        // todo: maybe move to a thread
        Chip8RunTimers(&app->chip8, GetTime() * 1000.0);

        if (Chip8ShouldPlayBeep(&app->chip8)) {
            PlaySound(app->beepSound);
        }
    
        if (Chip8ShouldDraw(&app->chip8)) break;
    }
}

int AppShouldRun(App *app) {
    return !WindowShouldClose() && Chip8ShouldRun(&app->chip8);
}

void AppDrawChip8DisplayBuffer(App *app) {
    BeginTextureMode(app->renderTarget);
        ClearBackground(BLACK);

        for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
                if ((app->chip8).displayBuffer[y * CHIP8_DISPLAY_WIDTH + x] != 0) {
                    DrawPixel(x, y, RAYWHITE);  
                }
            }
        }
    EndTextureMode();

    Chip8DrawFinished(&app->chip8);
}

void AppDraw(App *app) {
    BeginDrawing();
        RenderTexture2D target = app->renderTarget;

        DrawTexturePro(target.texture, 
            (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height },
            (Rectangle){ 0, 0, (float)target.texture.width * SCALE, (float)target.texture.height * SCALE },
            (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}

void AppDestroy(App *app) {
    CloseAudioDevice();
    UnloadRenderTexture(app->renderTarget);
    CloseWindow();
}
