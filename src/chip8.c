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

    memcpy(chip8->displayBuffer, rom, romSize * sizeof(uint8_t));

    chip8->shouldRun = 1;
}

int Chip8ShouldRun(Chip8 *chip8) {
    return chip8->shouldRun;
}

void Chip8RunTick(Chip8 *chip8, double timeMs) {
    // todo: process input
}
