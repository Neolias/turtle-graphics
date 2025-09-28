project(SaveLoadManagerModule VERSION 0.1 LANGUAGES CXX)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI SaveLoadManagerModule
    SOURCES
        src/SaveLoadManager.cpp
        src/SaveLoadManager.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

include_directories(${CMAKE_SOURCE_DIR}/src/modules/Turtle/src)
include_directories(${CMAKE_SOURCE_DIR}/src/modules/CLI/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick TurtleModuleplugin CLIModuleplugin)
