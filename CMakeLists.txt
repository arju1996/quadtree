# cmake_minimum_required(VERSION 3.29)
cmake_minimum_required(VERSION 4.0)
project(quad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


add_library(quad_warnings INTERFACE)
if(MSVC)
        target_compile_options(quad_warnings INTERFACE /W4 /permissive- /utf-8)
else()
        target_compile_options(quad_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_library(quadtree_core INTERFACE)
target_include_directories(quadtree_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

option(QUAD_BUILD_UI            "Build the raylib UI executable"        ON)
option(QUAD_BUILD_TESTS         "Build the tests"                       ON)


if(QUAD_BUILD_TESTS)
    add_executable(quad_test
            tests/test_helper.h
            tests/test_quadtree_creation.cpp
            tests/test_quadtree_remove.cpp
            tests/test_quadtree_neighbours.cpp
            tests/test_quadtree_main.cpp
    )    
    target_link_libraries(quad_test PRIVATE quadtree_core quad_warnings)
endif()

if(QUAD_BUILD_UI)
    include(FetchContent)
    # Uses an existing install if CMAKE_PREFIX_PATH points at one,
    # otherwise downloads and builds raylib.
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
        GIT_SHALLOW    ON
        FIND_PACKAGE_ARGS CONFIG
    )
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    set(RAY_LIB_GUI_WIN_FOLDER "F:/installed_tools/raylib/raygui" CACHE PATH "Path to raygui")
    if (RAY_LIB_GUI_WIN_FOLDER)
        set(RAYGUI_INCLUDE_DIR "${RAY_LIB_GUI_WIN_FOLDER}")
    else()
        # raygui is header-only
        FetchContent_Declare(raygui
            GIT_REPOSITORY https://github.com/raysan5/raygui.git
            GIT_TAG        4.0
            GIT_SHALLOW    ON
        )
        FetchContent_MakeAvailable(raygui)
        set(RAYGUI_INCLUDE_DIR "${raygui_SOURCE_DIR}/src")
    endif()
    FetchContent_MakeAvailable(raylib)
    add_executable(quad 
                # main.cc
                src/main_ui.cc
                )
    target_include_directories(quad SYSTEM PRIVATE ${RAYGUI_INCLUDE_DIR})
    target_link_libraries(quad PRIVATE quadtree_core quad_warnings raylib)
endif()