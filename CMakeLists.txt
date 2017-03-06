cmake_minimum_required(VERSION 3.6)
project(Simple_File_System)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES main.c)
add_executable(Simple_File_System ${SOURCE_FILES})