#-------------------------------------------------
#
# Project created by QtCreator 2017-05-28T11:07:57
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WuziQi
TEMPLATE = app


SOURCES += main.cpp\
        startwindow.cpp \
    ai.cpp \
    gamelogic.cpp \
    gameview.cpp \
    chessboard.cpp \
    gameenum.cpp \
    gamedata.cpp \
    gameconfig.cpp \
    board.cpp \
    settingwindow.cpp \
    aboutdialog.cpp

HEADERS  += startwindow.h \
    ai.h \
    gameenum.h \
    gamelogic.h \
    gamedata.h \
    gameview.h \
    gameconfig.h \
    chessboard.h \
    board.h \
    bluebutton.h \
    settingwindow.h \
    aboutdialog.h

FORMS    += startwindow.ui \
    gameview.ui \
    settingwindow.ui \
    aboutdialog.ui

RESOURCES += \
    game.qrc
RC_FILE = WuziQi.rc
