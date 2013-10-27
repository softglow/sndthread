#-------------------------------------------------
#
# Project created by QtCreator 2013-10-08T18:19:43
#
#-------------------------------------------------

QT       -= gui
QT       += multimedia

TARGET = sndthread
TEMPLATE = lib

DEFINES += SNDTHREAD_LIBRARY

SOURCES += sndthread.cpp \
    sndbuf.cpp \
    sndsnes.cpp \
    sndbloop.cpp \
    sndio.cpp

HEADERS += sndthread.h\
        sndthread_global.h \
    sndbuf.h \
    sndsnes.h \
    sndbloop.h \
    sndio.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
