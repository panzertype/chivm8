# Chivm8

```
 ██████╗██╗  ██╗██╗██╗   ██╗███╗   ███╗ █████╗ 
██╔════╝██║  ██║╚═╝██║   ██║████╗ ████║██╔══██╗
██║     ███████║██╗██║   ██║██╔████╔██║╚█████╔╝
██║     ██╔══██║██║╚██╗ ██╔╝██║╚██╔╝██║██╔══██╗
╚██████╗██║  ██║██║ ╚████╔╝ ██║ ╚═╝ ██║╚█████╔╝
 ╚═════╝╚═╝  ╚═╝╚═╝  ╚═══╝  ╚═╝     ╚═╝ ╚════╝ 
```

## Description

Chivm8 is a lightweight and accurate Chip8 emulator written in C99, utilizing the Raylib library for cross-platform graphics and audio support.

## Installation

### Prerequisites

- CMake (version 3.15 or higher)
- Raylib (version 5.5 or higher)

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/panzertype/chivm8.git
   cd chivm8
   ```

2. Configure the project with CMake:
   ```bash
   cmake -S . -B build
   ```

3. Build the executable:
   ```bash
   cd build
   make
   ```

The `chivm8` executable will be created in the `build` directory.

## Usage

Run the emulator by providing the path to a Chip8 ROM file:

```bash
./chivm8 path/to/your/rom.ch8
```

### Controls

The Chip8 keypad is mapped to the following keyboard keys:

```
Chip8 Keypad    Keyboard
   1 2 3 4       1 2 3 4
   Q W E R       Q W E R
   A S D F       A S D F
   Z X C V       Z X C V
```

- **ESC**: Close the emulator window

## Development

To build in debug mode with symbols:

```bash
cmake -DDEBUG=ON ..
make
```
