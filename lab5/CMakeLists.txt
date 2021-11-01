cmake_minimum_required(VERSION 3.19)
project(lab5 C)

set(CMAKE_C_STANDARD 99 -pthread)
set(CMAKE_C_FLAGS -pthread)

add_executable(lab5 main.c )
add_executable(lab5_2 prog2.c)
add_executable(lab5_2_threads prog2_threads.c)
add_executable(lab5_3 prog_3.c)
add_executable(lab5_3_threads prog3_threads.c)