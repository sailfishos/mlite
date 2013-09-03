TEMPLATE = app
TARGET = mlitenotificationtool
INCLUDEPATH += ../../src
DEPENDPATH += $$INCLUDEPATH

QMAKE_LIBDIR = ../../src
LIBS = -lmlite

SOURCES += mlitenotificationtool.cpp
INSTALLS += target
QT += dbus

target.path = /usr/bin
