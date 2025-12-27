COMPILER=clang
RAYLIB_INCLUDE=/opt/homebrew/Cellar/raylib/5.5/include
RAYLIB_LIB=/opt/homebrew/Cellar/raylib/5.5/lib

$COMPILER --std=c99 ./src/main.c ./src/chip8.c \
    -o chip8 \
    -Wall -Werror \
    -I$RAYLIB_INCLUDE -L$RAYLIB_LIB -lraylib
