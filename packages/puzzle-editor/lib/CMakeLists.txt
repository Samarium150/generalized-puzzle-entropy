file(GLOB HOG2_SOURCES
        hog2/utils/FPUtil.cpp
        hog2/utils/Graphics.cpp
        hog2/utils/SVGUtil.cpp
)
file(GLOB HOG2_HEADERS
        hog2/environments/Witness.h
        hog2/envutil/PuzzleInferenceRule.h
        hog2/generic/PuzzleEntropy.h
        hog2/gui/STUB/GL/gl.h
        hog2/gui/STUB/GL/glut.h
        hog2/search/Heuristic.h
        hog2/search/SearchEnvironment.h
        hog2/simulation/OccupancyInterface.h
        hog2/utils/FPUtil.h
        hog2/utils/GLUtil.h
        hog2/utils/Graphics.h
        hog2/utils/SVGUtil.h
        hog2/utils/vectorCache.h
)

if (EMSCRIPTEN)
    file(GLOB HOG2_GUI
            hog2/gui/Common.cpp
            hog2/gui/Trackball.cpp
            hog2/gui/STUB/GL/gl.cpp
            hog2/gui/web/html_hog.cpp
            hog2/utils/GLUtil.cpp
    )
    file(GLOB HOG2_GUI_HEADERS
            hog2/gui/Common.h
            hog2/gui/Trackball.h
    )
    list(APPEND HOG2_SOURCES ${HOG2_GUI})
    list(APPEND HOG2_HEADERS ${HOG2_GUI_HEADERS})
endif ()

add_library(hog2 OBJECT ${HOG2_SOURCES})
target_compile_definitions(hog2 PUBLIC NO_OPENGL)
target_compile_options(hog2 PRIVATE -pthread -w)
target_precompile_headers(hog2 PUBLIC ${HOG2_HEADERS})
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
