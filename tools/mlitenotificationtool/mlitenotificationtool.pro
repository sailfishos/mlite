TEMPLATE = app
TARGET = mlitenotificationtool
INCLUDEPATH += ../../src
DEPENDPATH += $$INCLUDEPATH

QMAKE_LIBDIR = ../../src
LIBS = -lmlite5

SOURCES += mlitenotificationtool.cpp
INSTALLS += target
QT = \
    core \
    dbus

target.path = /usr/bin
