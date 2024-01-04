file(GLOB HANDLERS impl/*.cpp)

add_executable(editor main.cpp ${HANDLERS})
target_include_directories(editor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(editor PRIVATE -pthread)
target_link_libraries(editor PRIVATE hog2)
target_link_options(editor PRIVATE
        -sEXPORTED_FUNCTIONS=["_InitHOG","_InitHOGArg","_DoFrame","_MouseEvent","_HitKey","_SetCanvasSize"]
        -sEXPORTED_RUNTIME_METHODS=["cwrap","stringToNewUTF8"]
        -sINITIAL_MEMORY=200MB
        -sMODULARIZE=1
        -sEXPORT_NAME=EditorModule
        -pthread
        -sPTHREAD_POOL_SIZE=4
)
