if (EMSCRIPTEN)
    return()
endif ()

add_executable(eval eval.cpp)
target_include_directories(eval PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(eval PRIVATE hog2)
