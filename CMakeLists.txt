cmake_minimum_required(VERSION 3.15)
project(chivm8)

set(CMAKE_C_STANDARD 99)

option(DEBUG "Build with debug symbols" OFF)

find_package(raylib 5.5 REQUIRED)

add_executable(${PROJECT_NAME} src/main.c src/chip8.c src/platform.c)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)

if(DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
endif()

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()