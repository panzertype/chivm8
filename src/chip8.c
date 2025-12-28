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

void Chip8UpdateKey(Chip8 *chip8, uint8_t key, KeyState state) {
    if (state == CHIP8_KEY_DOWN) {
        chip8->keyboard |= (1 << key);
    } else {
        chip8->keyboard &= ~(1 << key);
    }
}

KeyState Chip8GetKey(Chip8 *chip8, uint8_t key) {
    return (chip8->keyboard & (1 << key)) != 0 ? CHIP8_KEY_DOWN : CHIP8_KEY_UP;
}

void Chip8RunTick(Chip8 *chip8, double timeMs) {
    uint8_t firstByte = chip8->ram[chip8->programCounter];
    uint8_t secondByte = chip8->ram[chip8->programCounter + 1];

    uint16_t opcode = ((uint16_t)firstByte << 8) | (uint16_t)secondByte;

    printf("[INFO]: Received opcode: 0x%04X\n", opcode);

    switch ((opcode & 0xF000) >> 12) {
        case 0x4:
            printf("0x4 \n");
            break;
        default:
            fprintf(
                stderr,
                "[ERROR] Unknown opcode: 0x%04X\n",
                opcode
            );
            exit(1);
    }

    chip8->programCounter += 2;
}
