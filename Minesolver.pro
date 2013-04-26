#-------------------------------------------------
#
# Project created by QtCreator 2013-02-11T08:48:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Minesolver
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    MinefieldData.cpp \
    MinefieldDisplay.cpp \
    RandomNumbers.cpp \
    CellDisplay.cpp \
    PathChooser.cpp \
    MineStateMachine.cpp \
    MineStateAnalyzer.cpp

HEADERS  += MainWindow.h \
    MinefieldData.h \
    MinefieldDisplay.h \
    Encapsulation.h \
    RandomNumbers.h \
    CellDisplay.h \
    PathChooser.h \
    MineStateMachine.h \
    MineStateAnalyzer.h
