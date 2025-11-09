QT += core widgets

CONFIG += c++11

# 设置源文件编码为UTF-8
QMAKE_CXXFLAGS += /utf-8

TARGET = LinearTableVisualization
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    ArrayWidget.cpp \
    LinkedListWidget.cpp

HEADERS += \
    MainWindow.h \
    ArrayWidget.h \
    LinkedListWidget.h

# 设置输出目录
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build
MOC_DIR = $$PWD/build
RCC_DIR = $$PWD/build
UI_DIR = $$PWD/build

