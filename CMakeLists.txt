cmake_minimum_required(VERSION 3.7)
project(UDPserver)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c list.c list.h Generic.h Generic.c)
add_executable(UDPserver ${SOURCE_FILES})