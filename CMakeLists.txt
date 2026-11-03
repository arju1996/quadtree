# cmake_minimum_required(VERSION 3.29)
cmake_minimum_required(VERSION 4.0)
project(quad)

set(CMAKE_CXX_STANDARD 20)


add_executable(quad_test
        tests/test_helper.h
        tests/test_quadtree_creation.cpp
        tests/test_quadtree_remove.cpp
        tests/test_quadtree_neighbours.cpp
        tests/test_quadtree_main.cpp
)

target_include_directories(quad_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


option(BUILD_UI "Build the raylib UI executable" ON)
if(BUILD_UI)
        add_executable(quad 
                # main.cc
                src/main_ui.cc
                )

        set(RAY_LIB_GUI_WIN_FOLDER "F:/installed_tools/raylib/raygui/src" CACHE PATH "Path to raygui")
        find_package(raylib CONFIG REQUIRED)


        target_include_directories(quad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_include_directories(quad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils)
        target_include_directories(quad PRIVATE ${RAY_LIB_GUI_WIN_FOLDER})

        target_link_libraries(quad raylib )
        target_link_libraries(quad winmm.lib)
endif()