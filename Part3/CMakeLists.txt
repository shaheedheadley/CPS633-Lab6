# define project
cmake_minimum_required(VERSION 3.10)
project(Lab6Part3 C)

# setting c17 standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# define executable
add_executable(part3 main.c)
