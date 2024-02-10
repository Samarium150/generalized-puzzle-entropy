if (NOT EMSCRIPTEN)
    return()
endif ()

file(GLOB HANDLERS impl/*.cpp)

add_executable(editor editor.cpp ${HANDLERS})
target_include_directories(editor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(editor PRIVATE -pthread)
target_link_libraries(editor PRIVATE hog2)
target_link_options(editor PRIVATE
        -sEXPORTED_FUNCTIONS=["_InitHOG","_InitHOGArg","_DoFrame","_MouseEvent","_HitKey","_SetCanvasSize"]
        -sEXPORTED_RUNTIME_METHODS=["cwrap","stringToNewUTF8"]
        -sINITIAL_MEMORY=200MB
        -sMODULARIZE=1
        -sEXPORT_NAME=EditorModule
        -sPTHREAD_POOL_SIZE="navigator.hardwareConcurrency"
        -pthread
)

add_executable(playground playground.cpp)
target_include_directories(playground PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(playground PRIVATE -pthread)
target_link_libraries(playground PRIVATE hog2)
target_link_options(playground PRIVATE
        -sEXPORTED_FUNCTIONS=["_InitHOG","_InitHOGArg","_DoFrame","_MouseEvent","_HitKey","_SetCanvasSize"]
        -sEXPORTED_RUNTIME_METHODS=["cwrap","stringToNewUTF8"]
        -sINITIAL_MEMORY=200MB
        -sMODULARIZE=1
        -sEXPORT_NAME=PlaygroundModule
        -sPTHREAD_POOL_SIZE="navigator.hardwareConcurrency"
        -pthread
)
