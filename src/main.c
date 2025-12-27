#include <stdio.h>
#include "chip8.h"
#include "raylib.h"

#define SCALE 8

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

    InitWindow(CHIP8_DISPLAY_WIDTH * SCALE, CHIP8_DISPLAY_HEIGHT * SCALE, "Chip-8");

    Chip8 chip8;
    Chip8Init(&chip8, fileData, bytesRead);

    while (!WindowShouldClose() && Chip8ShouldRun(&chip8)) {
        double totalMs = GetTime() * 1000.0;
    
        Chip8RunTick(&chip8, totalMs);
    
        BeginDrawing();
            // 1 2 3 4 5 6

            // 1 2 3
            // 4 5 6
            
            // x=1 y=1
            for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
                for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
                    int bufferPos = y * CHIP8_DISPLAY_WIDTH + x;
                    if (chip8.displayBuffer[bufferPos] != 0) {
                        DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, RAYWHITE);  
                    } else {
                        DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, BLACK);  
                    }
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}