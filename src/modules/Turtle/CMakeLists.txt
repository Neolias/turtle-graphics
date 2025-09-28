project(TurtleModule VERSION 0.1 LANGUAGES CXX)

set_source_files_properties(resources/images/cursor_turtle.png PROPERTIES QT_RESOURCE_ALIAS cursor_turtle.png)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI TurtleModule
    QML_FILES
        Turtle.qml
    SOURCES
        src/turtlecontrol.cpp
        src/turtlecontrol.h
    RESOURCES
        resources/images/cursor_turtle.png
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

find_package(Qt6 REQUIRED COMPONENTS Gui)
include_directories(${CMAKE_SOURCE_DIR}/src/modules/Canvas/src ${CMAKE_SOURCE_DIR}/src/modules/Obstacle/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Gui CanvasModuleplugin ObstacleModuleplugin)
