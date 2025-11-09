cmake_minimum_required(VERSION 3.5)
project(LinearTableVisualization VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    main.cpp
    MainWindow.cpp
    ArrayWidget.cpp
    LinkedListWidget.cpp
)

set(HEADERS
    MainWindow.h
    ArrayWidget.h
    LinkedListWidget.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 设置UTF-8编码（Windows MSVC）
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets
)

