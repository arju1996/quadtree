# cmake_minimum_required(VERSION 3.29)
cmake_minimum_required(VERSION 3.1)
project(quad)

set(CMAKE_CXX_STANDARD 20)


set(RAY_LIB_FOLDER "/home/sreelatha/learn/frameworks/raylib/install/")

set(RAY_LIB_WIN_FOLDER "H:\\test\\raylib\\raylib\\out\\install\\x64-Debug")

add_library(raylib STATIC IMPORTED)
set_target_properties(raylib
PROPERTIES IMPORTED_LOCATION
${RAY_LIB_WIN_FOLDER}/lib/raylib.lib
)
include_directories(${RAY_LIB_WIN_FOLDER}/include)

set(RAY_LIB_GUI_FOLDER "/home/sreelatha/learn/frameworks/raygui/src/")
set(RAY_LIB_GUI_WIN_FOLDER "H:/test/raylib/raygui/src")

include_directories(${RAY_LIB_GUI_WIN_FOLDER})

add_executable(quad 
        # main.cc
        main_ui.cc
        # Math_helper.h
        Math_helper_2.h

        quad.h
        quad.cc
        )
target_link_libraries(quad raylib)
target_link_libraries(quad winmm.lib)
# add_executable(test test_main.cpp)
