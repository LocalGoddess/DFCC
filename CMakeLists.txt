cmake_minimum_required(VERSION 3.23)
project(DFCC C)

set(CMAKE_C_STANDARD 17)

include_directories(inc)

add_executable(DFCC src/main.c src/lexer.c src/parser.c) # For *.c source files only!!
