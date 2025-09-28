cmake_minimum_required(VERSION 3.16)

project(TestTurtle LANGUAGES CXX)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/modules/Canvas/src
    ${CMAKE_SOURCE_DIR}/src/modules/Turtle/src
    ${CMAKE_SOURCE_DIR}/src/modules/Obstacle/src)

enable_testing()

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} tst_testturtle.cpp)
add_test(NAME TestTurtle COMMAND TestTurtle)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Quick
    Qt6::Test
    CanvasModuleplugin
    TurtleModuleplugin
    ObstacleModuleplugin)

