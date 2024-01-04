cmake_minimum_required(VERSION 3.13)
project(puzzle-editor LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_subdirectory(src)
