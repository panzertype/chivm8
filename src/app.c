#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "chip8.h"
#include "platform.h"

#define SCALE 8
#define FPS CHIP8_DISPLAY_REFRESH_RATE_HZ
#define INSTRUCTIONS_PER_SECOND 800
#define INSTRUCTIONS_PER_FRAME (INSTRUCTIONS_PER_SECOND / FPS)
#define GET_KEY_STATE(key) PlatformIsKeyUp(key) ? CHIP8_KEY_UP : CHIP8_KEY_DOWN

typedef struct {
    Chip8 chip8;
    char* romFilePath;
    Platform* platform; 
} App;

void AppInit(App *app) {
    srand(time(NULL));
    app->platform = PlatformCreate();
}

void AppHandleCliArgs(App *app, int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: chivm8 <rom file>\n");
        exit(1);
    }

    app->romFilePath = argv[1]; // Safe
    if (!PlatformFileExists(app->romFilePath)) {
        fprintf(stderr, "[ERROR] File \"%s\" could not be found\n", argv[1]);
        exit(1);
    };
}

void AppLoadRomIntoChip8(App *app) {
    assert(app->romFilePath != NULL);

    int bytesRead = 0;
    unsigned char *fileData = PlatformLoadFileData(app->romFilePath, &bytesRead); {
        if (fileData == NULL) {
            fprintf(stderr, "[ERROR] File could not be read\n");
            exit(1);
        }

        Chip8Init(&app->chip8, fileData, bytesRead);
    } PlatformUnloadFileData(fileData);
}

void AppCreateWindow(App *app) {
    PlatformCreateWindow(app->platform, CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, "Chivm8", FPS);
    PlatformInitRenderTarget(app->platform, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
    PlatformInitSound(app->platform);
}

void AppProcessChip8Inputs(App *app) {
    Chip8UpdateKey(&app->chip8, 0x1, GET_KEY_STATE(PLATFORM_KEY_ONE));
    Chip8UpdateKey(&app->chip8, 0x2, GET_KEY_STATE(PLATFORM_KEY_TWO));
    Chip8UpdateKey(&app->chip8, 0x3, GET_KEY_STATE(PLATFORM_KEY_THREE));
    Chip8UpdateKey(&app->chip8, 0xC, GET_KEY_STATE(PLATFORM_KEY_FOUR));

    Chip8UpdateKey(&app->chip8, 0x4, GET_KEY_STATE(PLATFORM_KEY_Q));
    Chip8UpdateKey(&app->chip8, 0x5, GET_KEY_STATE(PLATFORM_KEY_W));
    Chip8UpdateKey(&app->chip8, 0x6, GET_KEY_STATE(PLATFORM_KEY_E));
    Chip8UpdateKey(&app->chip8, 0xD, GET_KEY_STATE(PLATFORM_KEY_R));

    Chip8UpdateKey(&app->chip8, 0x7, GET_KEY_STATE(PLATFORM_KEY_A));
    Chip8UpdateKey(&app->chip8, 0x8, GET_KEY_STATE(PLATFORM_KEY_S));
    Chip8UpdateKey(&app->chip8, 0x9, GET_KEY_STATE(PLATFORM_KEY_D));
    Chip8UpdateKey(&app->chip8, 0xE, GET_KEY_STATE(PLATFORM_KEY_F));

    Chip8UpdateKey(&app->chip8, 0xA, GET_KEY_STATE(PLATFORM_KEY_Z));
    Chip8UpdateKey(&app->chip8, 0x0, GET_KEY_STATE(PLATFORM_KEY_X));
    Chip8UpdateKey(&app->chip8, 0xB, GET_KEY_STATE(PLATFORM_KEY_C));
    Chip8UpdateKey(&app->chip8, 0xF, GET_KEY_STATE(PLATFORM_KEY_V));
}

void AppRunChip8(App *app) {
    for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
        Chip8RunCycle(&app->chip8);

        // todo: maybe move to a thread
        Chip8RunTimers(&app->chip8, PlatformGetTimeMs());

        if (Chip8ShouldPlayBeep(&app->chip8)) {
            PlatformPlayBeepSound(app->platform);
        }
    
        if (Chip8ShouldDraw(&app->chip8)) break;
    }
}

int AppShouldRun(App *app) {
    return !PlatformWindowShouldClose() && Chip8ShouldRun(&app->chip8);
}

void AppRenderChip8DisplayBuffer(App *app) {
    PlatformRenderDisplayBufferToRenderTarget(
        app->platform,
        (app->chip8).displayBuffer,
        CHIP8_DISPLAY_WIDTH,
        CHIP8_DISPLAY_HEIGHT
    );
}

void AppDraw(App *app) {
    PlatformDrawRenderTarget(app->platform, SCALE);
    Chip8DrawFinished(&app->chip8);
}

void AppDestroy(App *app) {
    PlatformDestroySound(app->platform);
    PlatformDestroyRenderTarget(app->platform);
    PlatformDestroyWindow(app->platform);
    PlatformDestroy(app->platform);
}
