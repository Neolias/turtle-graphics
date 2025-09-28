project(CanvasModule VERSION 0.1 LANGUAGES CXX)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI CanvasModule
    SOURCES
        src/canvas.hpp
        src/canvas.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

find_package(Qt6 REQUIRED COMPONENTS Gui)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Gui ObstacleModuleplugin)
include_directories(${CMAKE_SOURCE_DIR}/src/modules/Obstacle/src)
