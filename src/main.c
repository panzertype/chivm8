#include <stdio.h>
#include "chip8.h"
#include "raylib.h"

#define SCALE 8

#define FPS 60

#define CLOCK_SPEED 800
#define INSTRUCTIONS_PER_SECOND (CLOCK_SPEED / CHIP8_CYCLES_PER_INSTRUCTION)
#define INSTRUCTIONS_PER_FRAME (INSTRUCTIONS_PER_SECOND / FPS)

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

    Chip8 chip8;

    int bytesRead = 0;
    unsigned char *fileData = LoadFileData(romFilePath, &bytesRead); {
        if (fileData == NULL) {
            fprintf(stderr, "[ERROR] File could not be read\n");
            return 1;
        }

        Chip8Init(&chip8, fileData, bytesRead);
    } UnloadFileData(fileData);

    SetTargetFPS(FPS);
    InitWindow(CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, "Chip-8");

    RenderTexture2D target = LoadRenderTexture(CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

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

        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
            Chip8RunCycle(&chip8);

            // todo: maybe move to a thread
            Chip8RunTimers(&chip8, GetTime() * 1000.0);

            if (Chip8ShouldPlayBeep(&chip8)) {
                // todo: play sound
            }
        
            if (Chip8ShouldDraw(&chip8)) break;
        }

        BeginTextureMode(target);
            ClearBackground(BLACK);

            for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
                for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
                    if (chip8.displayBuffer[y * CHIP8_DISPLAY_WIDTH + x] != 0) {
                        DrawPixel(x, y, RAYWHITE);  
                    }
                }
            }
        EndTextureMode();

        BeginDrawing();
            DrawTexturePro(target.texture, 
                (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height },
                (Rectangle){ 0, 0, (float)target.texture.width * SCALE, (float)target.texture.height * SCALE },
                (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();

        Chip8DrawFinished(&chip8);
    }

    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}