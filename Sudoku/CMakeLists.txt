cmake_minimum_required(VERSION 3.10)
project(untitled11  C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS " -Wall -Wextra -Werror -pedantic-errors ")

add_executable(untitled11 main.c parser.c parser.h linkedList.c linkedList.h commands.c commands.h mainAux.c mainAux.h gurobi_c.h stack.c stack.h aaa.cpp)
target_link_libraries(untitled11 /Library/gurobi801/mac64/lib/libgurobi80.so)