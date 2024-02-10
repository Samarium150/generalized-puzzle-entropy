cmake_minimum_required(VERSION 3.24)
project(puzzle-editor LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (${CMAKE_CXX_COMPILER} MATCHES "em\\+\\+")
    set(EMSCRIPTEN TRUE)
else ()
    set(EMSCRIPTEN FALSE)
endif ()

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)
