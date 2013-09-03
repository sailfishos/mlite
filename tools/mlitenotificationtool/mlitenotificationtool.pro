TEMPLATE = app
TARGET = mlitenotificationtool
INCLUDEPATH += ../../src
DEPENDPATH += $$INCLUDEPATH

QMAKE_LIBDIR = ../../src
equals(QT_MAJOR_VERSION, 4): LIBS = -lmlite
equals(QT_MAJOR_VERSION, 5): LIBS = -lmlite5

SOURCES += mlitenotificationtool.cpp
INSTALLS += target
QT += dbus

target.path = /usr/bin
