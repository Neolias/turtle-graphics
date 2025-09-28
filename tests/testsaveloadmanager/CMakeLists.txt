cmake_minimum_required(VERSION 3.16)

project(TestSaveLoadManager LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/src/modules/Turtle/src)
include_directories(${CMAKE_SOURCE_DIR}/src/modules/SaveLoadManager/src)

enable_testing()

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestSaveLoadManager tst_testsaveloadmanager.cpp)
add_test(NAME TestSaveLoadManager COMMAND TestSaveLoadManager)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick Qt6::Test TurtleModuleplugin SaveLoadManagerModuleplugin)

