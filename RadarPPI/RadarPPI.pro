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

# C++17 标准 (Qt 6 要求)
CONFIG += c++17

# 禁用 Qt 6 moc 生成代码导致的编译警告
QMAKE_CXXFLAGS += -Wno-c++11-narrowing

# 源文件
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    radarwidget.cpp \
    trackplayer.cpp \
    trackcontrolpanel.cpp

# 头文件
HEADERS += \
    mainwindow.h \
    radarwidget.h \
    trackplayer.h \
    trackcontrolpanel.h

# UI 文件
FORMS += \
    mainwindow.ui \
    trackcontrolpanel.ui

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
