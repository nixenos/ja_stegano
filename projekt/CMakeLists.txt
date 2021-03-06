cmake_minimum_required(VERSION 3.5)

project(projekt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt5Widgets REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        src/logic.cpp src/logic.h src/dataStructures.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(projekt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET projekt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(projekt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(projekt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(projekt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(projekt PUBLIC ${CMAKE_SOURCE_DIR}/asm_src/libdata_modifications_asm.so)
target_link_libraries(projekt PUBLIC ${CMAKE_SOURCE_DIR}/c_src/libdata_modifications_c.so)
target_link_libraries(projekt PUBLIC ${CMAKE_DL_LIBS})

set_target_properties(projekt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(projekt)
endif()


add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/asm_src/libdata_modifications.so
                   COMMAND nasm -f elf64 -g data_modifications_xmm.asm -l data_modifications.lst -F dwarf && gcc -shared data_modifications_xmm.o -o libdata_modifications.so
                   DEPENDS ${CMAKE_SOURCE_DIR}/asm_src/data_modifications_xmm.asm
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/asm_src)
