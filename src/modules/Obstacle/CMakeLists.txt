project(ObstacleModule VERSION 0.1 LANGUAGES CXX)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI ObstacleModule
    SOURCES
        src/obstacle.hpp
        src/obstacle.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

find_package(Qt6 REQUIRED COMPONENTS Gui)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Gui)
