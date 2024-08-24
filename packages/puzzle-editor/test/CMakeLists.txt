if (EMSCRIPTEN)
    return()
endif ()

include(FetchContent)
FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG main
)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(CLI11)
FetchContent_MakeAvailable(json)

add_executable(eval eval.cpp)
target_compile_options(eval PRIVATE -pthread -Wall)
target_include_directories(eval PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_precompile_headers(eval REUSE_FROM hog2)
target_link_libraries(eval PRIVATE hog2 CLI11::CLI11)

add_executable(comp comp.cpp)
target_compile_options(comp PRIVATE -pthread -Wall)
target_include_directories(comp PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_precompile_headers(comp REUSE_FROM hog2)
target_link_libraries(comp PRIVATE hog2 nlohmann_json::nlohmann_json)
