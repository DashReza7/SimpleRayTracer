cmake_minimum_required(VERSION 3.28)
project(UCSD_Renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(UCSD_Renderer
        src/main.cpp
        src/util/VecMath.cpp
        src/Shape.cpp
        src/Scene.cpp)

include_directories(libs/FreeImage)

target_link_libraries(UCSD_Renderer
        ${CMAKE_SOURCE_DIR}/libs/FreeImage/FreeImage.lib)

add_custom_command(TARGET UCSD_Renderer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/libs/FreeImage/FreeImage.dll"
        $<TARGET_FILE_DIR:UCSD_Renderer>)

