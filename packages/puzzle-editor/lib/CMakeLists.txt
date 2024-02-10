file(GLOB HOG2_SOURCES
        hog2/utils/FPUtil.cpp
        hog2/utils/Graphics.cpp
        hog2/utils/SVGUtil.cpp
)

if (EMSCRIPTEN)
    file(GLOB HOG2_GUI
            hog2/gui/Common.cpp
            hog2/gui/Trackball.cpp
            hog2/gui/STUB/GL/gl.cpp
            hog2/gui/web/html_hog.cpp
            hog2/utils/GLUtil.cpp
    )
    list(APPEND HOG2_SOURCES ${HOG2_GUI})
endif ()

add_library(hog2 OBJECT ${HOG2_SOURCES})
target_compile_definitions(hog2 PUBLIC NO_OPENGL)
target_compile_options(hog2 PUBLIC -w)
target_include_directories(hog2 PUBLIC
        hog2/environments
        hog2/envutil
        hog2/generic
        hog2/gui
        hog2/gui/STUB/GL
        hog2/search
        hog2/simulation
        hog2/utils
)
