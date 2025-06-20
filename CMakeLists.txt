cmake_minimum_required(VERSION 3.10)
project(TinyShell C)

set(CMAKE_C_STANDARD 99)

set(SOURCES
    tinyshell.c
    history.c
    rawmode.c
    # Add more .c files as you create them
)

add_executable(tinyshell ${SOURCES})