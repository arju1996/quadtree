# cmake_minimum_required(VERSION 3.29)
cmake_minimum_required(VERSION 3.1)
project(quad)

set(CMAKE_CXX_STANDARD 20)


set(RAY_LIB_FOLDER "/home/sreelatha/learn/frameworks/raylib/install/")

add_library(raylib SHARED IMPORTED)
set_target_properties(raylib
PROPERTIES IMPORTED_LOCATION
${RAY_LIB_FOLDER}/lib/libraylib.so
)
include_directories(${RAY_LIB_FOLDER}/include)

set(RAY_LIB_GUI_FOLDER "/home/sreelatha/learn/frameworks/raygui/src/")

include_directories(${RAY_LIB_GUI_FOLDER})

add_executable(quad 
        # main.cc
        main_ui.cc
        # Math_helper.h
        Math_helper_2.h

        quad.h
        quad.cc
        )
target_link_libraries(quad raylib)
# add_executable(test test_main.cpp)
