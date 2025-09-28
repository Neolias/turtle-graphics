project(CLIModule VERSION 0.1 LANGUAGES CXX)

qt_add_qml_module(${PROJECT_NAME}
    STATIC
    URI CLIModule
    SOURCES
        src/CLI.cpp
        src/CLI.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

include_directories(${CMAKE_SOURCE_DIR}/src/modules/Parser/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick ParserModuleplugin)
