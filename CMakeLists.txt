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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)

add_executable(quad 
        # main.cc
        src/main_ui.cc
        )

add_executable(quad_test
        tests/test_helper.h
        tests/test_quadtree_creation.cpp
        tests/test_quadtree_remove.cpp
        tests/test_quadtree_neighbours.cpp
        tests/test_quadtree_main.cpp
)

target_link_libraries(quad raylib )
target_link_libraries(quad winmm.lib)