cmake_minimum_required(VERSION 3.16)

project(turtlegraphics VERSION 0.1 LANGUAGES CXX)

# Use this to change the C++ standard verstion
#set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the build type to Debug to include debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Add additional compiler flags for debugging
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

enable_testing()

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.6)

# Because modules are not in top folder//
    set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
    set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY}
    CACHE STRING "Import paths for Qt Creator's code model"
    FORCE
    )
# //

qt_add_executable(app${PROJECT_NAME}
    src/main.cpp
)

add_subdirectory(src/modules/Turtle)
add_subdirectory(src/modules/Parser)
add_subdirectory(src/modules/CLI)
add_subdirectory(src/modules/Canvas)
add_subdirectory(src/modules/Obstacle)
add_subdirectory(src/modules/SaveLoadManager)
add_subdirectory(tests/testturtle)
add_subdirectory(tests/testcanvas)
add_subdirectory(tests/testsaveloadmanager)

qt_add_qml_module(app${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES
        src/Main.qml
        src/CLIButtons.qml
        src/ObstacleControls.qml
        src/ControlPanel.qml
    RESOURCES
        src/resources/images/grid_background.png
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(app${PROJECT_NAME} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appturtlegraphics
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(app${PROJECT_NAME} PRIVATE
    Qt6::Quick
    TurtleModuleplugin
    ParserModuleplugin
    CLIModuleplugin
    ObstacleModuleplugin
    CanvasModuleplugin
    SaveLoadManagerModuleplugin
)

include(GNUInstallDirs)
install(TARGETS app${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Add a test that runs the application with Valgrind
find_program(VALGRIND "valgrind")
if(VALGRIND)
    enable_testing()

    # Define a memory check test using Valgrind
    add_test(NAME MemoryCheck
             COMMAND ${VALGRIND}
                     --show-leak-kinds=all       # Show all kinds of leaks (definitely lost, indirectly lost, etc.)
                     --track-origins=yes         # Track uninitialized memory origins
                     --error-exitcode=1          # Return non-zero on memory issues
                     $<TARGET_FILE:app${PROJECT_NAME}>
    )

    # Set properties for the Valgrind memory test
    set_tests_properties(MemoryCheck PROPERTIES
                         FAIL_REGULAR_EXPRESSION "ERROR SUMMARY: [^0] errors from [0-9]+ contexts"
                         LABELS "MemoryCheck"
                         TIMEOUT 120
    )
else()
    message(WARNING "Valgrind not found. No memory checks will be performed.")
endif()



