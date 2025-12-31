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

int Chip8ShouldDraw(Chip8 *chip8) {
    return chip8->shouldDraw;
}

void Chip8DrawFinished(Chip8 *chip8) {
    chip8->shouldDraw = 0;
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

static int Chip8DrawSprite(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t *sprite, uint8_t spriteLength) {
    chip8->shouldDraw = 1;

    int somePixelsErased = 0;
    
    // wrap if x > 63 and/or y > 31
    x = x % 64;
    y = y % 32;

    for (int spriteY = 0; spriteY < spriteLength; spriteY++) {
        if((y + spriteY) >= CHIP8_DISPLAY_HEIGHT) continue; // offscreen

        int bufferPos = (y + spriteY) * CHIP8_DISPLAY_WIDTH + x;

        uint8_t spriteHorizontalLine = sprite[spriteY];

        for (int spriteX = 0; spriteX < 8; spriteX++) {
            if((x + spriteX) >= CHIP8_DISPLAY_WIDTH) continue; // offscreen

            int pixelPos = bufferPos + spriteX;

            int wasPixelActive = chip8->displayBuffer[pixelPos] != 0;
            int isPixelActive = spriteHorizontalLine & (1 << (7 - spriteX));
            int isCollision = isPixelActive && wasPixelActive;

            if (isCollision) {
                chip8->displayBuffer[pixelPos] = 0;
            } else if (isPixelActive) {
                chip8->displayBuffer[pixelPos] = 1;
            }

            if (!somePixelsErased) {
                somePixelsErased = isCollision;
            }
        }
    }

    return somePixelsErased;
}

int Chip8RunTimers(Chip8 *chip8, double totalTimeMs) {
    if (totalTimeMs - chip8->latestTimersUpdateTimeMs < CHIP8_TIMER_RATE_MS) {
       return 0;
    }

    if (chip8->delay > 0) chip8->delay--;
    if (chip8->sound > 0) chip8->sound--;

    chip8->latestTimersUpdateTimeMs = totalTimeMs;
    return 1;
}

int Chip8ShouldPlayBeep(Chip8 *chip8) {
    return chip8->sound > 0;
}

void Chip8RunCycle(Chip8 *chip8) {
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
                case 0x00EE:
                    // The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
                    chip8->programCounter = chip8->stack[chip8->stackPointer - 1];
                    chip8->stackPointer--;
                    break;
                default:
                    goto unknownOpcode;
            }
            break;
        case 1: // 1nnn
            chip8->programCounter = nnn;
            break;
        case 2: // 2nnn
            // The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
            chip8->stackPointer++; 
            chip8->stack[chip8->stackPointer - 1] = chip8->programCounter;
            chip8->programCounter = nnn;
            break;
        case 3: // 3xkk
            // Skip next instruction if Vx == kk.
            if (chip8->registers[x] == kk) {
                chip8->programCounter += 2;
            }
            break;
        case 4: // 4xkk
            // Skip next instruction if Vx != kk.
            if (chip8->registers[x] != kk) {
                chip8->programCounter += 2;
            }
            break;
        case 5: // 5xy0
            if (n != 0x0) {
                goto unknownOpcode;
            }
            // Skip next instruction if Vx == Vy.
            if (chip8->registers[x] == chip8->registers[y]) {
                chip8->programCounter += 2;
            }
            break;
        case 6: // 6xkk
            chip8->registers[x] = kk;
            break;
        case 7: // 7xkk
            // Vx = Vx + kk.
            chip8->registers[x] += kk;
            break;
        case 8:
            switch(n) {
                case 0: // 8xy0
                    chip8->registers[x] = chip8->registers[y];
                    break;
                case 1: // 8xy1
                    chip8->registers[x] |= chip8->registers[y];

                    // @QUIRK: vF reset
                    chip8->registers[0xF] = 0;
                    break;
                case 2: // 8xy2
                    chip8->registers[x] &= chip8->registers[y];

                    // @QUIRK: vF reset
                    chip8->registers[0xF] = 0;
                    break;
                case 3: // 8xy3
                    chip8->registers[x] ^= chip8->registers[y];

                    // @QUIRK: vF reset
                    chip8->registers[0xF] = 0;
                    break;
                case 4: { // 8xy4
                    uint16_t result = chip8->registers[x] + chip8->registers[y];

                    // is carry
                    chip8->registers[0xF] = result > 255;

                    chip8->registers[x] = (uint8_t)result;

                    break;
                }
                case 5: { // 8xy5
                    // is no borrow
                    chip8->registers[0xF] = chip8->registers[x] > chip8->registers[y];
                    chip8->registers[x] -= chip8->registers[y];

                    break;
                }
                case 6: { // 8xy6
                    // @QUIRK: shifting
                    chip8->registers[x] = chip8->registers[y];
                    // is least significant bit one
                    chip8->registers[0xF] = chip8->registers[x] & 1;
                    // divide by 2
                    chip8->registers[x] >>= 1;

                    break;
                }
                case 7: { // 8xy7
                    // is no borrow
                    chip8->registers[0xF] = chip8->registers[y] > chip8->registers[x];
                    chip8->registers[x] = chip8->registers[y] - chip8->registers[x];

                    break;
                }
                case 0xE: { // 8xyE
                    // @QUIRK: shifting
                    chip8->registers[x] = chip8->registers[y];
                    // is most significant bit one
                    chip8->registers[0xF] = chip8->registers[x] >> 7;
                    // multiply by 2
                    chip8->registers[x] <<= 1;

                    break;
                }
                default:
                    goto unknownOpcode;
            }
            break;
        case 9: // 9xy0
            if (n != 0x0) {
                goto unknownOpcode;
            }

            // Skip next instruction if Vx != Vy.
            if (chip8->registers[x] != chip8->registers[y]) {
                chip8->programCounter += 2;
            }
            break;
        case 0xA: // Annn
            chip8->I = nnn;
            break;
        case 0xB: // Bnnn
            // Jump to location nnn + V0.
            chip8->programCounter = nnn + chip8->registers[0];
            break;
        case 0xD: { // Dxyn
            // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
            // Sprites may be up to 15 bytes, for a possible sprite size of 8x15.
            // Each row of 8 pixels is read as bit-coded starting from memory location I;
            // Sprites are XORed onto the existing screen. If this causes any pixels to be erased,
            // VF is set to 1, otherwise it is set to 0.

            uint8_t spriteLength = n;
            uint8_t vx = chip8->registers[x];
            uint8_t vy = chip8->registers[y];

            uint8_t sprite[15] = {0}; 
            memcpy(sprite, chip8->ram + chip8->I, spriteLength);

            int somePixelsErased = Chip8DrawSprite(chip8, vx, vy, sprite, spriteLength);

            chip8->registers[0xF] = somePixelsErased ? 1 : 0;

            break;
        }
        case 0xE:
            switch(kk) {
                case 0x9E:
                    // Skip next instruction if key with the value of Vx is pressed.
                    if (Chip8GetKey(chip8, chip8->registers[x]) == CHIP8_KEY_DOWN) {
                        chip8->programCounter += 2;
                    } 
                    break;
                case 0xA1:
                    // Skip next instruction if key with the value of Vx is not pressed.
                    if (Chip8GetKey(chip8, chip8->registers[x]) == CHIP8_KEY_UP) {
                        chip8->programCounter += 2;
                    } 
                    break;
                default:
                    goto unknownOpcode;
            }
            break;
        case 0xF:
            switch(kk) {
                case 0x07: // Fx07
                    // Vx = delay timer value.
                    chip8->registers[x] = chip8->delay;
                    break;
                case 0x0A: // Fx0A
                    // Wait for a key press, store the value of the key in Vx.
                    if (Chip8GetKey(chip8, chip8->registers[x]) == CHIP8_KEY_DOWN) {
                        chip8->registers[x] = x;
                    } else {
                        chip8->programCounter -= 2;
                    }
                    break;
                case 0x1E: // Fx1E 
                    // I = I + Vx.
                    chip8->I += chip8->registers[x];
                    break; 
                case 0x15: // Fx15
                    // delay timer = Vx.
                    chip8->delay = chip8->registers[x];
                    break;
                case 0x55: // Fx55 
                    // Copy the values of registers V0 through Vx into memory, starting at the address in I.
                    for (int i = 0; i <= x; i++) {
                        chip8->ram[chip8->I + i] = chip8->registers[i];
                    }

                    // @QUIRK: memory
                    chip8->I = chip8->I + x + 1;
                    break; 
                case 0x65: // Fx65
                    // Reads values from memory starting at location I into registers V0 through Vx.
                    for (int i = 0; i <= x; i++) {
                        chip8->registers[i] = chip8->ram[chip8->I + i];
                    }

                    // @QUIRK: memory
                    chip8->I = chip8->I + x + 1;
                    break;
                default:
                    goto unknownOpcode;
            }
            break;
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
