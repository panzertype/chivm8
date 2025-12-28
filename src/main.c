#include <stdio.h>
#include "chip8.h"
#include "raylib.h"

#define SCALE 8

#define GET_KEY_STATE(key) IsKeyUp(key) ? CHIP8_KEY_UP : CHIP8_KEY_DOWN

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: chip8 <rom file>\n");
        return 1;
    }

    char* romFilePath = argv[1];
    if (!FileExists(romFilePath)) {
        fprintf(stderr, "[ERROR] File \"%s\" could not be found\n", argv[1]);
        return 1;
    };

    int bytesRead = 0;
    unsigned char *fileData = LoadFileData(romFilePath, &bytesRead);
    if (fileData == NULL) {
        fprintf(stderr, "[ERROR] File could not be read\n");
        return 1;
    }

    Chip8 chip8;
    Chip8Init(&chip8, fileData, bytesRead);

    UnloadFileData(fileData);

    InitWindow(CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, "Chip-8");

    while (!WindowShouldClose() && Chip8ShouldRun(&chip8)) {
        Chip8UpdateKey(&chip8, 0x1, GET_KEY_STATE(KEY_ONE));
        Chip8UpdateKey(&chip8, 0x2, GET_KEY_STATE(KEY_TWO));
        Chip8UpdateKey(&chip8, 0x3, GET_KEY_STATE(KEY_THREE));
        Chip8UpdateKey(&chip8, 0xC, GET_KEY_STATE(KEY_FOUR));

        Chip8UpdateKey(&chip8, 0x4, GET_KEY_STATE(KEY_Q));
        Chip8UpdateKey(&chip8, 0x5, GET_KEY_STATE(KEY_W));
        Chip8UpdateKey(&chip8, 0x6, GET_KEY_STATE(KEY_E));
        Chip8UpdateKey(&chip8, 0xD, GET_KEY_STATE(KEY_R));

        Chip8UpdateKey(&chip8, 0x7, GET_KEY_STATE(KEY_A));
        Chip8UpdateKey(&chip8, 0x8, GET_KEY_STATE(KEY_S));
        Chip8UpdateKey(&chip8, 0x9, GET_KEY_STATE(KEY_D));
        Chip8UpdateKey(&chip8, 0xE, GET_KEY_STATE(KEY_F));

        Chip8UpdateKey(&chip8, 0xA, GET_KEY_STATE(KEY_Z));
        Chip8UpdateKey(&chip8, 0x0, GET_KEY_STATE(KEY_X));
        Chip8UpdateKey(&chip8, 0xB, GET_KEY_STATE(KEY_C));
        Chip8UpdateKey(&chip8, 0xF, GET_KEY_STATE(KEY_V));

        double totalMs = GetTime() * 1000.0;
    
        Chip8RunTick(&chip8, totalMs);
    
        BeginDrawing();
            for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
                for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
                    int bufferPos = y * CHIP8_DISPLAY_WIDTH + x;
                    int isPixelActive = chip8.displayBuffer[bufferPos] != 0;
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, isPixelActive ? RAYWHITE : BLACK);  
                }
            }
        EndDrawing();

        WaitTime(0.02);
    }

    CloseWindow();

    return 0;
}