cmake_minimum_required(VERSION 3.19)
project(lab6_inet C)

set(CMAKE_C_STANDARD 99)

add_executable(client client.c)
add_executable(server server.c)