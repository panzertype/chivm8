#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_DISPLAY_BUFFER_SIZE CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT

#define CLOCK_RATE(hz) (1 / (0.0 + hz))
static const double CHIP8_TIMER_RATE_MS = CLOCK_RATE(60);
#define CHIP8_CYCLES_PER_INSTRUCTION 1

// MEMORY MAP
#define MMAP_PROGRAM_END 0xFFF
#define MMAP_PROGRAM_START 0x200
#define MMAP_INTERPRETER_END 0x1FF
#define MMAP_INTERPRETER_START 0x000

#define CHIP8_MAX_PROGRAM_SIZE MMAP_PROGRAM_END - MMAP_PROGRAM_START

typedef struct {
   int shouldRun;
   int shouldDraw;

   // DEVICES
   uint8_t displayBuffer[CHIP8_DISPLAY_BUFFER_SIZE];
   uint8_t ram[4096];
   uint16_t keyboard;

   // REGISTERS
   uint16_t I;
   uint8_t registers[16];
   uint8_t delay;
   uint8_t sound;

   // TIMERS
   double latestTimersUpdateTimeMs;

   // PSEUDO-REGISTERS: not accessible from programs
   uint16_t programCounter;
   uint8_t stackPointer;

   uint16_t stack[16]; // up to 16 levels of nested subroutines
} Chip8;

typedef enum {
   CHIP8_KEY_UP,
   CHIP8_KEY_DOWN,
} KeyState;

int Chip8ShouldRun(Chip8 *chip8);
int Chip8ShouldDraw(Chip8 *chip8);
void Chip8DrawFinished(Chip8 *chip8);
int Chip8ShouldPlayBeep(Chip8 *chip8);
void Chip8Init(Chip8 *chip8, uint8_t *rom, int romSize);
void Chip8RunCycle(Chip8 *chip8);
int Chip8RunTimers(Chip8 *chip8, double totalTimeMs);
void Chip8UpdateKey(Chip8 *chip8, uint8_t key, KeyState state);
KeyState Chip8GetKey(Chip8 *chip8, uint8_t key);

static const uint8_t digitSprites[] = {
   0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
   0x20, 0x60, 0x20, 0x20, 0x70, // 1
   0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
   0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
   0x90, 0x90, 0xF0, 0x10, 0x10, // 4
   0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
   0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
   0xF0, 0x10, 0x20, 0x40, 0x40, // 7
   0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
   0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
   0xF0, 0x90, 0xF0, 0x90, 0x90, // A
   0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
   0xF0, 0x80, 0x80, 0x80, 0xF0, // C
   0xE0, 0x90, 0x90, 0x90, 0xE0, // D
   0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
   0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

#endif