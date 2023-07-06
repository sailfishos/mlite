TEMPLATE = app
TARGET = mliteremoteaction

QT = \
    core \
    dbus

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

QMAKE_LIBDIR = $$OUT_PWD/../../src
LIBS = -lmlite$${QT_MAJOR_VERSION}

SOURCES += main.cpp

target.path = /usr/libexec

INSTALLS += target
