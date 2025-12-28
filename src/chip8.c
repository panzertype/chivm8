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
    printf("[INFO] Rom size: %d\n", romSize);

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

static void Chip8ClearScreen(Chip8 *chip8) {
    memset(chip8->displayBuffer, 0, CHIP8_DISPLAY_BUFFER_SIZE);
}

void Chip8RunTick(Chip8 *chip8, double timeMs) {
    uint8_t firstByte = chip8->ram[chip8->programCounter];
    uint8_t secondByte = chip8->ram[chip8->programCounter + 1];

    uint16_t opcode = ((uint16_t)firstByte << 8) | (uint16_t)secondByte;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;
    uint16_t nnn = opcode & 0x0FFF;

    printf("[INFO]: Received opcode: 0x%04X ", opcode);
    printf("( x=0x%X, y=0x%X, kk=0x%02X, n=0x%X, nnn=0x%03X )\n", x, y, kk, n, nnn);

    chip8->programCounter += 2;

    switch ((opcode & 0xF000) >> 12) {
        case 0:
            switch(opcode) {
                case 0x00E0:
                    Chip8ClearScreen(chip8);
                    break;
                // case 0x00EE:
                //     // The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
                //     chip8->programCounter = chip8->stack[chip8->stackPointer - 1];
                //     chip8->stackPointer--;
                //     break;
                default:
                    goto unknownOpcode;
            }
            break;
        case 1: // 1nnn
            chip8->programCounter = nnn;
            break;
        // case 2: // 2nnn
        //     // The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
        //     chip8->stackPointer++; 
        //     chip8->stack[chip8->stackPointer - 1] = chip8->programCounter;
        //     chip8->programCounter = nnn;
        //     break;
        // case 4: // 4xkk
        //     // Skip next instruction if Vx != kk.
        //     if (chip8->registers[x] != kk) {
        //         chip8->programCounter += 2;
        //     }
        //     break;
        case 6: // 6xkk
            chip8->registers[x] = kk;
            break;
        case 7: // 7xkk
            // Vx = Vx + kk.
            chip8->registers[x] += kk;
            break;
        case 0xA: // Annn
            chip8->I = nnn;
            break;
        case 0xD: { // Dxyn
            // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
            // Sprites may be up to 15 bytes, for a possible sprite size of 8x15.
            // Each row of 8 pixels is read as bit-coded starting from memory location I;

            uint8_t spriteLength = n;
            uint8_t vx = chip8->registers[x];
            uint8_t vy = chip8->registers[y];

            uint8_t sprite[15] = {0}; 
            memcpy(sprite, chip8->ram + chip8->I, spriteLength);
            
            for (int spriteIdx = 0; spriteIdx < spriteLength; spriteIdx++) {
                int bufferPos = (vy + spriteIdx) * CHIP8_DISPLAY_WIDTH + vx;
                uint8_t spriteLine = sprite[spriteIdx];

                for (int bitPos = 0; bitPos < 8; bitPos++) {
                    int isPixelActive = spriteLine & (1 << (7 - bitPos));
                    chip8->displayBuffer[bufferPos + bitPos] = isPixelActive ? 1 : 0;
                }
            }

            // todo: set F register

            break;
        }
        // case 0xF:
        //     switch(kk) {
        //         case 0x1E: // Fx1E 
        //             // I = I + Vx.
        //             chip8->I += chip8->registers[x];
        //             break; 
        //         default:
        //             goto unknownOpcode;
        //     }
        //     break;
        default:
            unknownOpcode:
            fprintf(
                stderr,
                "[ERROR] Unknown opcode: 0x%04X\n",
                opcode
            );
            exit(1);
    }
}
