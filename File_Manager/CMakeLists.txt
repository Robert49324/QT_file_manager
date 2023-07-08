cmake_minimum_required(VERSION 3.5)

project(File_Manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    about.cpp
    about.h
    about.ui
)

set(File_Manager_ICON "logo.ico")

# Add resources
qt_add_resources(RESOURCES logo.qrc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(File_Manager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${RESOURCES}
    )
else()
    if(ANDROID)
        add_library(File_Manager SHARED
            ${PROJECT_SOURCES}
            ${RESOURCES}
        )
    else()
        add_executable(File_Manager WIN32
            ${PROJECT_SOURCES}
            ${RESOURCES}
        )
    endif()
endif()

target_link_libraries(File_Manager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(File_Manager PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_ICON_FILE logo.ico
)

install(TARGETS File_Manager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(File_Manager)
endif()
