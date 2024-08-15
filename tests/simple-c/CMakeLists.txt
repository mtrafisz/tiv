cmake_minimum_required(VERSION 3.15)
project(sample)

set (C_STANDARD_REQUIRED C11)

add_library(lib STATIC src/lib.c)
add_executable(sample src/main.c)
target_link_libraries(sample lib)
