#-------------------------------------------------
#
# Project: RadarPPI - 雷达P型显示仿真系统
# Version: 1.0
# Qt Version: 5.12.0
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadarPPI
TEMPLATE = app

# C++11 标准
CONFIG += c++11

# 源文件
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    radarwidget.cpp

# 头文件
HEADERS += \
    mainwindow.h \
    radarwidget.h

# UI 文件
FORMS += \
    mainwindow.ui

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
