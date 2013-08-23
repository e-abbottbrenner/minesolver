#-------------------------------------------------
#
# Project created by QtCreator 2013-02-11T08:48:47
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Minesolver
TEMPLATE = app

include(src/ui/ui.pri)
include(src/game/game.pri)
include(src/solver/solver.pri)
include(src/utils/utils.pri)

CONFIG(test) {
include(test/test.pri)
} else {
include(src/main/main.pri)
}
