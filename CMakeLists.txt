cmake_minimum_required(VERSION 3.22)
project(DFCC C)

set(CMAKE_C_STANDARD 17)

include_directories(inc)

add_executable(DFCC src/main.c src/lexer.c) # For *.c source files only!!
