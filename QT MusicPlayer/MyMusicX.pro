#-------------------------------------------------
#
# Project created by QtCreator 2013-11-22T07:29:50
#
#-------------------------------------------------

QT       += core gui phonon

TARGET = MyMusicX
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myplaylist.cpp \
    no_foucus_delegate.cpp \
    MySystemTray.cpp \
    mylrc.cpp

HEADERS  += mainwindow.h \
    myplaylist.h \
    no_focus_delegate.h \
    MySystemTray.h \
    mylrc.h

FORMS    += mainwindow.ui

RESOURCES += \
    myres.qrc
RC_FILE =  myapp.rc
