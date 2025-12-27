#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

void Chip8Init(Chip8 *chip8, uint8_t *rom, int romSize) {
    if (romSize > CHIP8_MAX_PROGRAM_SIZE) {
        fprintf(
            stderr,
            "[ERROR] Rom size is larger than maximum allowed program size: %d > %d \n",
            romSize,
            CHIP8_MAX_PROGRAM_SIZE
        );
        exit(1);
    }

    memset(chip8, 0, sizeof(Chip8));

    memcpy(chip8->ram + MMAP_INTERPRETER_START, digitSprites, sizeof(digitSprites));
    memcpy(chip8->ram + MMAP_PROGRAM_START, rom, romSize * sizeof(uint8_t));

    chip8->shouldRun = 1;
    chip8->programCounter = MMAP_PROGRAM_START;
}

int Chip8ShouldRun(Chip8 *chip8) {
    return chip8->shouldRun;
}

void Chip8RunTick(Chip8 *chip8, double timeMs) {
    // todo: process input

    uint8_t firstByte = chip8->ram[chip8->programCounter];
    uint8_t secondByte = chip8->ram[chip8->programCounter + 1];

    printf("%X %X\n", firstByte, secondByte);

    chip8->programCounter += 2;
}
