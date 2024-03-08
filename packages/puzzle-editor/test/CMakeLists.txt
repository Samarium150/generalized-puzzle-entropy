if (EMSCRIPTEN)
    return()
endif ()

add_executable(eval eval.cpp)
target_compile_options(eval PRIVATE -pthread -Wall)
target_include_directories(eval PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_precompile_headers(eval REUSE_FROM hog2)
target_link_libraries(eval PRIVATE hog2)
